#include <levos/levos.h>
#include <stdarg.h>

int sched_ticks = 0;

void __printk_emit(char c)
{
	if (c)
		console_send(c);
}

void printk_emit(char *s)
{
	int i;
	for (i = 0; i < strlen(s); i++)
		__printk_emit(s[i]);
}

void vprintk(char *fmt, va_list ap)
{
	char *s = 0;
	int i;

	for (i = 0; i < strlen(fmt); i++)
	{
		if (fmt[i] == '%') {
			switch(fmt[i + 1]) {
				case 's':
					s = va_arg(ap, char *);
					printk_emit(s);
					i ++;
					break;
				case 'd': {
					int c = va_arg(ap, int);
					char str[32] = {0};
					itoa(c, 10, str);
					printk_emit(str);
					i ++;
					break;
				}
				case 'x': {
					int c = va_arg(ap, int);
					char str[32] = {0};
					itoa(c, 16, str);
					printk_emit(str);
					i++;
					break;
				}
				case 'c': {
					char c = (char)(va_arg(ap, int) & ~0xFFFFFF00);
					__printk_emit(c);
					i ++;
					break;
				}
			}
		} else {
			__printk_emit(fmt[i]);
		}
	}
}

void panic(char *fmt, ...)
{
	va_list ap;
	
	DISABLE_IRQ();

	va_start(ap, fmt);

	printk("Kernel panic: ");
	vprintk(fmt, ap);

	va_end(ap);

	while(1);
}

void printk_ticks()
{
	int i;
	char str[32] = {0};
	vprintk("[", NULL);
	sched_ticks = arch_get_ticks();
	itoa(sched_ticks, 10, str);
	for (i = strlen(str); i < 6; i++)
		vprintk("0", NULL);
	vprintk(str, NULL);
	vprintk("] ", NULL);
}

void printk(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	printk_ticks();
	vprintk(fmt, ap);
	
	va_end(ap);
}
