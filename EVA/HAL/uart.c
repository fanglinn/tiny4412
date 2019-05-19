#include "uart.h"


int tstc(void)
{
	return ((UTRSTAT0 & (1<<0)) == 1);
}

char getc(void)
{
	char c;
	/* ��ѯ״̬�Ĵ�����ֱ������Ч���� */
	while (!(UTRSTAT0 & (1<<0)));
	
	c = URXH0; /* ��ȡ���ռĴ�����ֵ */
		
	return c;
}

void putc(char c)
{
    /* ��ѯ״̬�Ĵ�����ֱ�����ͻ���Ϊ�� */
    while (!(UTRSTAT0 & (1<<2)));
    
    UTXH0 = c; /* д�뷢�ͼĴ��� */
    
    return;
}

void puts(const char *s)
{
    while (*s)
    {
        putc(*s);
        s++;
    }
}


void uart_init(void)
{
	/* 1.������Ӧ��GPIO���ڴ��ڹ��� */
    GPA0CON = 0x22222222;
    GPA1CON = 0x222222;

    /* 2.����UARTʱ��ԴSCLK_UART */
    /* 2.1 CLK_SRC_DMC  : bit[12]��MUX_MPLL_SEL=1, SCLKMPLLLʹ��MPLL�����
     * 2.2 CLK_SRC_TOP1 : bit[12]��MUX_MPLL_USER_SEL_T=1, MUXMPLLʹ��SCLKMPLLL
     * 2.3 CLK_SRC_PERIL0 : bit[3:0]��UART0_SEL=6, MOUTUART0ʹ��SCLKMPLL_USER_T
     * ����, MOUTUART0������MPLL�����, 800MHz
     */
    /*
     *   PWM_SEL = 0;
     *   UART5_SEL = 0;
     *   UART4_SEL = 6;      // ����ʱ��Դѡ SCLKMPLL_USER_T
     *   UART3_SEL = 6;
     *   UART2_SEL = 6;
     *   UART1_SEL = 6;
     *   UART0_SEL = 6;
     */
    CLK_SRC_PERIL0 = ((0 << 24) | (0 << 20) | (6 << 16) | (6 << 12) | (6<< 8) | (6 << 4)    | (6));

    /*
     �� ��Ƶϵ�� = 7+1 = 8
     * 2.4 CLK_DIV_PERIL0 : bit[3:0]��UART0_RATIO=7,����SCLK_UART0=MOUTUART0/(7+1)=100MHz
     */
    CLK_DIV_PERIL0 = ((7 << 20) | (7 << 16) | (7 << 12) | (7 << 8) | (7 << 4) | (7));

    /* 3.���ô���0��� */
    /* ����FIFO�жϴ�����ֵ
     * ʹ��FIFO
     */
    UFCON0 = 0x111;
    
    /* �������ݸ�ʽ: 8n1, ��8������λ,û�н���λ,1��ֹͣλ */
    ULCON0 = 0x3;
    
    /* �������ж�/��ѯģʽ
     * ��һ����DMAģʽ,���²�ʹ��
     */
    UCON0  = 0x5;
    
    /* SCLK_UART0=100MHz, ����������Ϊ115200
     * �Ĵ�����ֵ���¼���:
     *    DIV_VAL   = 100,000,000 / (115200 * 16)   - 1 = 53.25
     *    UBRDIVn0  = �������� = 53
     *    UFRACVAL0 = С������ x 16 = 0.25 * 16 = 4
     */
    UBRDIV0 = 53;
    UFRACVAL0 = 4;
}

#if 0
void itoa(int n, char s[], int fmt)
{
	int i;
	int j;
	int sign;

	sign = n;	 //��¼����
	if(sign < 0)
	{
		n = -n;	//��Ϊ�������� 
	}

	i = 0;
	do{
		s[i++] = n % fmt + '0';	//ȡ��һ������
	}while((n /= fmt) > 0);

	if(sign < 0 )
	{
		s[i++] = '-';
		s[i] = '\0';
	}

	for(j = i; j >= 0; j-- )
	{
		putc(s[j]);
	}
}


#define va_list char*   /* �ɱ������ַ */
#define va_start(ap, x) ap=(char*)&x+sizeof(x) /* ��ʼ��ָ��ָ���һ���ɱ���� */
#define va_arg(ap, t)   (ap+=sizeof(t),*((t*)(ap-sizeof(t)))) /* ȡ�ò���ֵ��ͬʱ�ƶ�ָ��ָ��������� */
#define va_end(ap)  ap=0 /* ������������ */

void printf(const char *fmt, ...)
{
	va_list ap;
	char *p, *sval;
	int ival;
	double dval;
	char buffer[64] = { 0 };
	
	va_start(ap, fmt);
	for (p = fmt; *p; p++) {
	if(*p != '%') {
		putc(*p);
		continue;
	}

	switch(*++p) {
		case 'x':			
		case 'd':		
			ival = va_arg(ap, int);
			itoa(ival, buffer, 10);
			char *pstr = buffer;
			while (*pstr) putc(*pstr++);
			break;
		case 'f':
			dval = va_arg(ap, double);
			printf("%f", dval);
			break;
		case 's':
			for (sval = va_arg(ap, char *); *sval; sval++)
				putc(*sval);
			break;
		default:
			putc(*p);
			break;
		}
	}
	va_end(ap);
}
#endif
