/*
 * Davicom DM96xx USB 10/100Mbps ethernet devices
 *
 * Peter Korsgaard <jacmet@sunsite.dk>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

/* #define DEBUG */

#include <common.h>
#include <usb.h>
#include <linux/mii.h>
#include "usb_ether.h"
#include <malloc.h>
#include <errno.h>

#include <asm/unaligned.h>
#include <memalign.h>
#include <dm.h>
#include <miiphy.h>

#include <command.h>
#include <net.h>
#include <asm/io.h>


/* datasheet:
 http://ptm2.cc.utu.fi/ftp/network/cards/DM9601/From_NET/DM9601-DS-P01-930914.pdf
*/

#define DM9601_BASE_NAME "DM9621A"

struct dm_dongle {
   unsigned short vendor;
   unsigned short product;
   int flags;
};

static const struct dm_dongle products[] = {
   {0x07aa, 0x9601, 0, }, /* Corega FEther USB-TXC */
   {0x0a46, 0x9601, 0, }, /* Davicom USB-100 */
   {0x0a46, 0x6688, 0, }, /* ZT6688 USB NIC */
   {0x0a46, 0x0268, 0, }, /* ShanTou ST268 USB NIC */
   {0x0a46, 0x8515, 0, }, /* ADMtek ADM8515 USB NIC */
   {0x0a47, 0x9601, 0, }, /* Hirose USB-100 */
   {0x0fe6, 0x8101, 0, }, /* DM9601 USB to Fast Ethernet Adapter */
   {0x0fe6, 0x9700, 0, }, /* DM9601 USB to Fast Ethernet Adapter */
   {0x0a46, 0x9000, 0, }, /* DM9000E */
   {0x0a46, 0x9620, 0, }, /* DM9620 USB to Fast Ethernet Adapter */
   {0x0a46, 0x9621, 0, }, /* DM9621A USB to Fast Ethernet Adapter */
   {0x0a46, 0x9622, 0, }, /* DM9622 USB to Fast Ethernet Adapter */
   {0x0a46, 0x0269, 0, }, /* DM962OA USB to Fast Ethernet Adapter */
   {0x0a46, 0x1269, 0, }, /* DM9621A USB to Fast Ethernet Adapter */
   {},         /* END */
};


/* control requests */
#define DM_READ_REGS   0x00
#define DM_WRITE_REGS  0x01
#define DM_READ_MEMS   0x02
#define DM_WRITE_REG   0x03
#define DM_WRITE_MEMS  0x05
#define DM_WRITE_MEM   0x07

/* registers */
#define DM_NET_CTRL    0x00
#define DM_RX_CTRL 0x05
#define DM_SHARED_CTRL 0x0b
#define DM_SHARED_ADDR 0x0c
#define DM_SHARED_DATA 0x0d    /* low + high */
#define DM_PHY_ADDR    0x10    /* 6 bytes */
#define DM_MCAST_ADDR  0x16    /* 8 bytes */
#define DM_GPR_CTRL    0x1e
#define DM_GPR_DATA    0x1f
#define DM_CHIP_ID 0x2c
#define DM_MODE_CTRL   0x91    /* only on dm9620 */

/* DM_RX_CTRL */
#define DM_RX_DIS_LONG (1 << 5)
#define DM_RX_DIS_CRC  (1 << 4)
#define DM_RX_ALL  (1 << 3)
#define DM_RX_PRMSC    (1 << 1)
#define DM_RX_RXEN (1 << 0)

/* chip id values */
#define ID_DM9601  0
#define ID_DM9620  1

#define DM_MAX_MCAST   64
#define DM_MCAST_SIZE  8
#define DM_EEPROM_LEN  256
#define DM_TX_OVERHEAD 2   /* 2 byte header */
#define DM_RX_OVERHEAD 7   /* 3 byte header + 4 byte crc tail */
#define DM_TIMEOUT 1000

#define USB_CTRL_SET_TIMEOUT 5000
#define USB_CTRL_GET_TIMEOUT 5000
#define USB_BULK_SEND_TIMEOUT 5000
#define USB_BULK_RECV_TIMEOUT 5000

#define AX_RX_URB_SIZE 2048
#define PHY_CONNECT_TIMEOUT 5000

static int dm_read(struct ueth_data *dev, u8 reg, u16 length, void *data)
{
   int len;
   debug("%s() reg=%d, length=%d\n", __func__, reg, length);
   len = usb_control_msg(
       dev->pusb_dev,
       usb_rcvctrlpipe(dev->pusb_dev, 0),
       DM_READ_REGS,
       USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
       0,
       reg,
       data,
       length,
       USB_CTRL_SET_TIMEOUT);

   return len == length ? 0 : -EINVAL;
}

static int dm_read_reg(struct ueth_data *dev, u8 reg, u8 *value)
{
   int res;
   ALLOC_CACHE_ALIGN_BUFFER(u8, v, 2);

   res = dm_read(dev, reg, 1, v);
   *value = v[0];
   return res;
}

static int dm_write(struct ueth_data *dev, u8 reg, u16 length, void *data)
{
   int len;
   debug("%s() reg=%d, length=%d data=%pM\n", __func__, reg, length, data);
   len = usb_control_msg(
       dev->pusb_dev,
       usb_sndctrlpipe(dev->pusb_dev, 0),
       DM_WRITE_REGS,
       USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
       0,
       reg,
       data,
       length,
       USB_CTRL_SET_TIMEOUT);
   return len == length ? 0 : -EINVAL;
}

static int dm_write_reg(struct ueth_data *dev, u8 reg, u8 value)
{
   debug("%s() reg=%d, value=0x%x\n", __func__, reg, (int)value);
   int res = usb_control_msg(
       dev->pusb_dev,
       usb_sndctrlpipe(dev->pusb_dev, 0),
       DM_WRITE_REG,
       USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
       value,
       reg,
       NULL,
       0,
       USB_CTRL_SET_TIMEOUT);
   return res;
}


static int dm_read_shared_word(struct ueth_data *dev, int phy, u8 reg, __le16 *value)
{
   int ret, i;

   /*mutex_lock(&dev->phy_mutex);*/

   dm_write_reg(dev, DM_SHARED_ADDR, phy ? (reg | 0x40) : reg);
   dm_write_reg(dev, DM_SHARED_CTRL, phy ? 0xc : 0x4);

   for (i = 0; i < DM_TIMEOUT; i++) {
       u8 tmp = 0;

       udelay(1);
       ret = dm_read_reg(dev, DM_SHARED_CTRL, &tmp);
       if (ret < 0)
           goto out;

       /* ready */
       if ((tmp & 1) == 0)
           break;
   }

   if (i == DM_TIMEOUT) {
       printf("%s read timed out!\n", phy ? "phy" : "eeprom");
       ret = -EIO;
       goto out;
   }

   dm_write_reg(dev, DM_SHARED_CTRL, 0x0);
   ret = dm_read(dev, DM_SHARED_DATA, 2, value);

   debug("read shared %d 0x%02x returned 0x%04x, %d\n",
          phy, reg, *value, ret);

 out:
   /*mutex_unlock(&dev->phy_mutex);*/
   return ret;
}


static int dm_write_shared_word(struct ueth_data *dev, int phy, u8 reg, __le16 value)
{
   int ret, i;
   ALLOC_CACHE_ALIGN_BUFFER(__le16, v, 1);
   *v = value;

   /*mutex_lock(&dev->phy_mutex);*/

   ret = dm_write(dev, DM_SHARED_DATA, 2, v);
   if (ret < 0)
       goto out;

   dm_write_reg(dev, DM_SHARED_ADDR, phy ? (reg | 0x40) : reg);
   if (!phy) dm_write_reg(dev, DM_SHARED_CTRL, 0x10);
   dm_write_reg(dev, DM_SHARED_CTRL, phy ? 0x0a : 0x12);
   dm_write_reg(dev, DM_SHARED_CTRL, 0x10);

   for (i = 0; i < DM_TIMEOUT; i++) {
       u8 tmp = 0;

       udelay(1);
       ret = dm_read_reg(dev, DM_SHARED_CTRL, &tmp);
       if (ret < 0)
           goto out;

       /* ready */
       if ((tmp & 1) == 0)
           break;
   }

   if (i == DM_TIMEOUT) {
       printf("%s write timed out!\n", phy ? "phy" : "eeprom");
       ret = -EIO;
       goto out;
   }

   dm_write_reg(dev, DM_SHARED_CTRL, 0x0);

out:
   /*mutex_unlock(&dev->phy_mutex);*/
   return ret;
}


#if 0
static int dm_read_eeprom_word(struct ueth_data *dev, u8 offset, void *value)
{
   return dm_read_shared_word(dev, 0, offset, value);
}


static int dm9601_get_eeprom_len(struct net_device *dev)
{
   return DM_EEPROM_LEN;
}


static int dm9601_get_eeprom(struct net_device *net,
                struct ethtool_eeprom *eeprom, u8 * data)
{
   struct usbnet *dev = netdev_priv(net);
   __le16 *ebuf = (__le16 *) data;
   int i;

   /* access is 16bit */
   if ((eeprom->offset % 2) || (eeprom->len % 2))
       return -EINVAL;

   for (i = 0; i < eeprom->len / 2; i++) {
       if (dm_read_eeprom_word(dev, eeprom->offset / 2 + i,
                   &ebuf[i]) < 0)
           return -EINVAL;
   }
   return 0;
}
#endif


static int dm9601_mdio_read(struct ueth_data *dev, int phy_id, int loc)
{
   ALLOC_CACHE_ALIGN_BUFFER(__le16, v, 2);

   dm_read_shared_word(dev, phy_id, loc, v);

   debug("dm9601_mdio_read() phy_id=0x%02x, loc=0x%02x, returns=0x%04x|0x%04x\n",
         phy_id, loc, *v, le16_to_cpu(*v));

   return le16_to_cpu(*v);
}


static int dm9601_mdio_write(struct ueth_data *dev, int phy_id, int loc,
                 int val)
{
   __le16 res = cpu_to_le16(val);
  int mdio_val;

   debug("dm9601_mdio_write() phy_id=0x%02x, loc=0x%02x, val=0x%04x|0x%04x\n",
          phy_id, loc, res, val);

   dm_write_shared_word(dev, phy_id, loc, res);
   mdelay(1);
   mdio_val = dm9601_mdio_read(dev, phy_id, loc);
   return mdio_val;
}


#if 0
static void dm9601_get_drvinfo(struct net_device *net,
                  struct ethtool_drvinfo *info)
{
   /* Inherit standard device info */
   usbnet_get_drvinfo(net, info);
   info->eedump_len = DM_EEPROM_LEN;
}

static u32 dm9601_get_link(struct net_device *net)
{
   struct usbnet *dev = netdev_priv(net);

   return mii_link_ok(&dev->mii);
}

static int dm9601_ioctl(struct net_device *net, struct ifreq *rq, int cmd)
{
   struct usbnet *dev = netdev_priv(net);

   return generic_mii_ioctl(&dev->mii, if_mii(rq), cmd, NULL);
}
#endif


static int dm9601_set_mac_address(struct eth_device *eth)
{
   struct ueth_data *dev = (struct ueth_data *)eth->priv;
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buf, ETH_ALEN);

   if (!is_valid_ethaddr(eth->enetaddr)) {
       printf("not setting invalid mac address %pM\n", eth->enetaddr);
       return -EINVAL;
   }

   memcpy(buf, eth->enetaddr, ETH_ALEN);
   dm_write(dev, DM_PHY_ADDR, ETH_ALEN, buf);

   return 0;
}


static int dm9601_read_mac_address(struct eth_device *eth)
{
   struct ueth_data *dev = (struct ueth_data *)eth->priv;
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buf, ETH_ALEN);

   /* read MAC */
   if (dm_read(dev, DM_PHY_ADDR, ETH_ALEN, buf) < 0) {
       printf("dm9601: Error reading MAC address\n");
       return -ENODEV;
   }

   memcpy(eth->enetaddr, buf, ETH_ALEN);

   return 0;
}


static void dm9601_set_multicast(struct ueth_data *dev, u8 mcast[DM_MCAST_SIZE])
{
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, hashes, DM_MCAST_SIZE);
   u8 rx_ctl = (DM_RX_DIS_LONG | DM_RX_DIS_CRC | DM_RX_RXEN);

   memcpy(hashes, mcast, DM_MCAST_SIZE);

#if 0
   if (net->flags & IFF_PROMISC) {
       rx_ctl |= DM_RX_PRMSC;
   } else if (net->flags & IFF_ALLMULTI ||
          netdev_mc_count(net) > DM_MAX_MCAST) {
       rx_ctl |= DM_RX_ALL;
   } else if (!netdev_mc_empty(net)) {
       struct netdev_hw_addr *ha;

       netdev_for_each_mc_addr(ha, net) {
           u32 crc = ether_crc(ETH_ALEN, ha->addr) >> 26;
           hashes[crc >> 3] |= 1 << (crc & 0x7);
       }
   }
#endif

   dm_write(dev, DM_MCAST_ADDR, DM_MCAST_SIZE, hashes);
   dm_write_reg(dev, DM_RX_CTRL, rx_ctl);
}


static int dm9601_link_reset(struct ueth_data *dev)
{
   u8 mcast0[DM_MCAST_SIZE] = { 0x0 };
   u8 mcast1[DM_MCAST_SIZE] = { 0, 0x00, 0, 0x80, 0, 0, 0, 0 };
   u8 mcast2[DM_MCAST_SIZE] = { 0, 0x00, 0, 0x84, 0, 0, 0, 0 };
   u8 mcast3[DM_MCAST_SIZE] = { 0, 0x80, 0, 0x84, 0, 0, 0, 0 };

   dm9601_set_multicast(dev, mcast0);
   dm9601_set_multicast(dev, mcast1);
   dm9601_set_multicast(dev, mcast2);
   dm9601_set_multicast(dev, mcast3);

   return 0;
}

/*
 * mii_nway_restart - restart NWay (autonegotiation) for this interface
 *
 * Returns 0 on success, negative on error.
 */
static int mii_nway_restart(struct ueth_data *dev)
{
   int bmcr;
   int r = -1;

   /* if autoneg is off, it's an error */
   bmcr = dm9601_mdio_read(dev, dev->phy_id, MII_BMCR);
   debug("%s: bmcr: 0x%x\n", __func__, bmcr);
   if (bmcr & BMCR_ANENABLE) {
       bmcr |= BMCR_ANRESTART;
       dm9601_mdio_write(dev, dev->phy_id, MII_BMCR, bmcr);
       r = 0;
   }

   return r;
}


#if 0
/*
 * mcs7830_set_autoneg() - setup and trigger ethernet link autonegotiation
 * @eth:   network device to run link negotiation on
 * Return: zero upon success, negative upon error
 *
 * the routine advertises available media and starts autonegotiation
 */
static int dm_set_autoneg(struct ueth_data *dev)
{
   int adv, flg;
   int rc;

   debug("%s()\n", __func__);

   /*
    * algorithm taken from the Linux driver, which took it from
    * "the original mcs7830 version 1.4 driver":
    *
    * enable all media, reset BMCR, enable auto neg, restart
    * auto neg while keeping the enable auto neg flag set
    */

   adv = ADVERTISE_PAUSE_CAP | ADVERTISE_ALL | ADVERTISE_CSMA;
   rc = dm9601_mdio_write(dev, dev->phy_id, MII_ADVERTISE, adv);

   flg = 0;
   if (!rc)
       rc = dm9601_mdio_write(dev, dev->phy_id, MII_BMCR, flg);

   flg |= BMCR_ANENABLE;
   if (!rc)
       rc = dm9601_mdio_write(dev, dev->phy_id, MII_BMCR, flg);

   flg |= BMCR_ANRESTART;
   if (!rc)
       rc = dm9601_mdio_write(dev, dev->phy_id, MII_BMCR, flg);

   debug("%s() rc: %d\n", __func__, rc);

   return rc;
}
#endif


static int dm9601_init(struct eth_device *eth, bd_t *bd)
{
   struct ueth_data    *dev = (struct ueth_data *)eth->priv;
   int timeout = 0;
   int link_detected;

   debug("** %s()\n", __func__);

   mii_nway_restart(dev);
   /*dm_set_autoneg(dev);*/

   dm9601_link_reset(dev);

#define TIMEOUT_RESOLUTION 50  /* ms */
   do {
       link_detected = dm9601_mdio_read(dev, dev->phy_id, MII_BMSR) &
           BMSR_LSTATUS;
       if (!link_detected) {
           if (timeout == 0)
               printf("Waiting for Ethernet connection... ");
           udelay(TIMEOUT_RESOLUTION * 1000);
           timeout += TIMEOUT_RESOLUTION;
       }
   } while (!link_detected && timeout < PHY_CONNECT_TIMEOUT);
   if (link_detected) {
       if (timeout != 0)
           printf("done.\n");
   } else {
       printf("unable to connect.\n");
       goto out_err;
   }
#undef TIMEOUT_RESOLUTION

   return 0;

out_err:
   printf("dm9601: Error: unable to init device.\n");
   return -1;
}


static int dm9601_send(struct eth_device *eth, void *packet, int length)
{
   struct ueth_data *dev = (struct ueth_data *)eth->priv;
   int err;
   u16 packet_len;
   int actual_len;
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, msg, PKTSIZE + sizeof(packet_len));

   debug("** %s(), len %d\n", __func__, length);

   /* format:
      b1: packet length low
      b2: packet length high
      b3..n: packet data
   */

   packet_len = length;
   cpu_to_le16s(&packet_len);

   memcpy(msg, &packet_len, sizeof(packet_len));
   memcpy(msg + sizeof(packet_len), (void *)packet, length);

   err = usb_bulk_msg(dev->pusb_dev,
               usb_sndbulkpipe(dev->pusb_dev, dev->ep_out),
               (void *)msg,
               length + sizeof(packet_len),
               &actual_len,
               USB_BULK_SEND_TIMEOUT);
   debug("Tx: len = %u, actual = %u, err = %d\n",
           length + sizeof(packet_len), actual_len, err);

   return err;
}


static int dm9601_recv(struct eth_device *eth)
{
   struct ueth_data *dev = (struct ueth_data *)eth->priv;
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, recv_buf, AX_RX_URB_SIZE);
   ALLOC_CACHE_ALIGN_BUFFER(unsigned char, pkt, PKTSIZE);
   unsigned char *buf_ptr;
   int err;
   int actual_len;
   u16 packet_len;
   u8 status;

   debug("** %s()\n", __func__);

   /* format:
      b1: rx status
      b2: packet length (incl crc) low
      b3: packet length (incl crc) high
      b4..n-4: packet data
      bn-3..bn: ethernet crc
    */

   err = usb_bulk_msg(dev->pusb_dev,
               usb_rcvbulkpipe(dev->pusb_dev, dev->ep_in),
               (void *)recv_buf,
               AX_RX_URB_SIZE,
               &actual_len,
               USB_BULK_RECV_TIMEOUT);
   debug("Rx: len = %u, actual = %u, err = %d\n", AX_RX_URB_SIZE,
       actual_len, err);
   if (err != 0) {
       printf("Rx: failed to receive\n");
       return -1;
   }
   if (actual_len > AX_RX_URB_SIZE) {
       printf("Rx: received too many bytes %d\n", actual_len);
       return -1;
   }

   buf_ptr = recv_buf;
   while (actual_len > 0) {
       /*
        * First byte contains packet status.
        */
       if (actual_len < sizeof(status)) {
           debug("Rx: incomplete packet length (status)\n");
           return -1;
       }
       status = buf_ptr[0];
       buf_ptr += sizeof(status);
       actual_len -= sizeof(status);

       if (unlikely(status & 0xbf)) {
           printf("Rx: packet status failure: %d\n", (int)status);
           /*
           if (status & 0x01) dev->net->stats.rx_fifo_errors++;
           if (status & 0x02) dev->net->stats.rx_crc_errors++;
           if (status & 0x04) dev->net->stats.rx_frame_errors++;
           if (status & 0x20) dev->net->stats.rx_missed_errors++;
           if (status & 0x90) dev->net->stats.rx_length_errors++;
           */
           return -1;
       }

       /*
        * 2nd and 3rd bytes contain the length of the actual data.
        * Extract the length of the data.
        */
       if (actual_len < sizeof(packet_len)) {
           debug("Rx: incomplete packet length (size)\n");
           return -1;
       }
       memcpy(&packet_len, buf_ptr, sizeof(packet_len));
       le16_to_cpus(&packet_len);
       packet_len -= 4;
       buf_ptr += sizeof(packet_len);
       actual_len -= sizeof(packet_len);

       if (packet_len > actual_len) {
           printf("Rx: too large packet: %d, actual: %d\n", packet_len, actual_len);
           return -1;
       }

       /* Notify net stack */
       memcpy(pkt, buf_ptr, packet_len);
       net_process_received_packet(pkt, packet_len);

       /* Adjust for next iteration. Packets are padded to 16-bits */
       /*if (packet_len & 1)
           packet_len++;*/
       actual_len -= (packet_len + 4);
       buf_ptr += packet_len + 4;
   }

   return err;
}


static void dm9601_halt(struct eth_device *eth)
{
   debug("** %s()\n", __func__);
}


void dm9601_eth_before_probe(void)
{
   debug("** %s()\n", __func__);
}


int dm9601_eth_probe(struct usb_device *dev, unsigned int ifnum,
       struct ueth_data* ss)
{
   struct usb_interface *iface;
   struct usb_interface_descriptor *iface_desc;
   int ep_in_found = 0, ep_out_found = 0;
   int i;

   debug("\n****************** %s *******************\n", __func__);

   /* let's examine the device now */
   iface = &dev->config.if_desc[ifnum];
   iface_desc = &dev->config.if_desc[ifnum].desc;

   for (i = 0; products[i].vendor != 0; i++) {
       debug("\n******************dev->descriptor.idVendor = %#04x *******************\n", dev->descriptor.idVendor);
       debug("\n******************dev->descriptor.idProduct = %#04x *******************\n", dev->descriptor.idProduct);
       if (dev->descriptor.idVendor == products[i].vendor &&
           dev->descriptor.idProduct == products[i].product)
           /* Found a supported dongle */
           break;
   }

   if (products[i].vendor == 0)
       {
       debug("\n****************** products[i].vendor == 0 *******************\n");
       return 0;
       }

   memset(ss, 0, sizeof(struct ueth_data));

   /* At this point, we know we've got a live one */
   debug("\n\nUSB Ethernet device detected: %#04x:%#04x\n",
         dev->descriptor.idVendor, dev->descriptor.idProduct);

   /* Initialize the ueth_data structure with some useful info */
   ss->ifnum = ifnum;
   ss->pusb_dev = dev;
   ss->subclass = iface_desc->bInterfaceSubClass;
   ss->protocol = iface_desc->bInterfaceProtocol;

   /*
    * We are expecting a minimum of 3 endpoints - in, out (bulk), and
    * int. We will ignore any others.
    */
   for (i = 0; i < iface_desc->bNumEndpoints; i++) {
       /* is it an BULK endpoint? */
       if ((iface->ep_desc[i].bmAttributes &
            USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK) {
           u8 ep_addr = iface->ep_desc[i].bEndpointAddress;
           if (ep_addr & USB_DIR_IN) {
               if (!ep_in_found) {
                   ss->ep_in = ep_addr &
                       USB_ENDPOINT_NUMBER_MASK;
                   ep_in_found = 1;
               }
           } else {
               if (!ep_out_found) {
                   ss->ep_out = ep_addr &
                       USB_ENDPOINT_NUMBER_MASK;
                   ep_out_found = 1;
               }
           }
       }

       /* is it an interrupt endpoint? */
       if ((iface->ep_desc[i].bmAttributes &
           USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT) {
           ss->ep_int = iface->ep_desc[i].bEndpointAddress &
               USB_ENDPOINT_NUMBER_MASK;
           ss->irqinterval = iface->ep_desc[i].bInterval;
       }
   }
   debug("Endpoints In %d Out %d Int %d\n",
         ss->ep_in, ss->ep_out, ss->ep_int);

   /* Do some basic sanity checks, and bail if we find a problem */
   if (usb_set_interface(dev, iface_desc->bInterfaceNumber, 0) ||
       !ss->ep_in || !ss->ep_out || !ss->ep_int) {
       printf("Problems with device\n");
       return 0;
   }

   return 1;
}


int dm9601_eth_get_info(struct usb_device *usb_dev, struct ueth_data *ss,
               struct eth_device *eth)
{
   u8 id = 0xff;
   u8 mcast0[DM_MCAST_SIZE] = { 0x0 };

   debug("\n%s\n", __func__);

   if (!eth) {
       printf("%s: missing parameter.\n", __func__);
       return 0;
   }

   sprintf(eth->name, "%s%d", DM9601_BASE_NAME, 0 /*curr_eth_dev++*/);
   eth->init = dm9601_init;
   eth->send = dm9601_send;
   eth->recv = dm9601_recv;
   eth->halt = dm9601_halt;
#ifdef CONFIG_MCAST_TFTP
   /*
   eth->mcast = dm9601_mcast(struct eth_device *, const u8 *enetaddr, u8 set);
   */
#endif
   eth->write_hwaddr = dm9601_set_mac_address;
   eth->priv = ss;

   /* reset */
   dm_write_reg(ss, DM_NET_CTRL, 1);
   udelay(20);

   /* read MAC */
   if (dm9601_read_mac_address(eth))
       return 0;
   debug("\nDavicom DM96xx MAC address is %pM\n", eth->enetaddr);

   /*
    * Overwrite the auto-generated address only with good ones.
    */
   if (!is_valid_ethaddr(eth->enetaddr)) {
       printf("dm9601: No valid MAC address in EEPROM, using %pM\n",
           eth->enetaddr);
       /*__dm9601_set_mac_address(ss);*/
   }

   if (dm_read_reg(ss, DM_CHIP_ID, &id) < 0) {
       printf("dm9601: Error reading chip ID\n");
       return 0;
   }

   debug("Chip ID = %d\n", id);
   /* put dm9620 devices in dm9601 mode */
   if (id == ID_DM9620) {
       u8 mode;

       if (dm_read_reg(ss, DM_MODE_CTRL, &mode) < 0) {
           printf("dm9601: Error reading MODE_CTRL\n");
           return 0;
       }
       dm_write_reg(ss, DM_MODE_CTRL, mode & 0x7f);
   }

   /* power up phy */
   dm_write_reg(ss, DM_GPR_CTRL, 1);
   dm_write_reg(ss, DM_GPR_DATA, 0);

   /* receive broadcast packets */
   dm9601_set_multicast(ss, mcast0);

   dm9601_mdio_read(ss, ss->phy_id, MII_BMSR);
   dm9601_mdio_write(ss, ss->phy_id, MII_BMCR, BMCR_RESET);
   dm9601_mdio_write(ss, ss->phy_id, MII_ADVERTISE,
             ADVERTISE_ALL | ADVERTISE_CSMA | ADVERTISE_PAUSE_CAP);
   mii_nway_restart(ss);

   return 1;
}

