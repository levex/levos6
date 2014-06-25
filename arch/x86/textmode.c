#include <levos/levos.h>

struct tmpriv {
	int x;
	int y;
	int col;
};

#define TM_PRIV(x) ((struct tmpriv *)((x)->priv))

void textmode_clear(struct device *dev)
{
	struct tmpriv *p = TM_PRIV(dev);

	for (int i = 0; i < 2000; i++)
		*(uint16_t *)(0xB8000 + i * 2) = p->col << 8 | ' ';
}

void __textmode_scrollup(struct tmpriv *p)
{
	for (int y = 0; y < 25; y++)
		memcpy((void *)(0xB8000 + y*80*2),
			(void *)(0xB8000 + (y+1)*80*2),
			80*2);
	memset((void *)(0xB8000 + 80*25*2), p->col << 8 | ' ', 80*2);
	return;
}

void __textmode_putchar(struct tmpriv *p, char c)
{
	if (!p || !c)
		return;
	
	if (p->x > 80 || c == '\n') {
		p->y ++;
		p->x = 0;
	}

	if (p->y >= 25) {
		__textmode_scrollup(p);
		p->x = 0;
		//p->y --;
		p->y = 0;
	}

	if (c == '\n')
		return;
	
	*(uint16_t *)(0xB8000 + 80*2*(p->y) + (p->x)*2) = (p->col << 8) | c;
	p->x ++;
	return;
}

int textmode_write(struct device *dev, char *buf, size_t count, size_t po)
{	
	struct tmpriv *p = TM_PRIV(dev);
	int i;

	for (i = 0; i < count; i++)
		__textmode_putchar(p, buf[i]);
	
	return i;
}

struct device tm_dev = {
	.write = (void *)textmode_write,
	.name = "tm",
};

struct console tm_con = {
	.dev = &tm_dev,
};

struct tmpriv tp = {
	.x = 0,
	.y = 0,
	.col = 0x07,
};

int textmode_init()
{
	tm_dev.priv = &tp;
	
	textmode_clear(&tm_dev);

	console_register(&tm_con);
	return 0;
}
