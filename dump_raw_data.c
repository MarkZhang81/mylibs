#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**** copy this part to code ***********************************************/

#define myprint  printf		/* change it to the proper function */

static unsigned int bytes_per_line = 16;
static void print_line(int nline, const void *buf, int len, int unit)
{
	char tmp[128] = {};
	const uint8_t *p8 = buf;
	const uint16_t *p16 = buf;
	const uint32_t *p32 = buf;
	int i;


	switch (unit) {
	case 1:
		sprintf(tmp, "%04x:", nline * bytes_per_line);
		for (i = 0; i < len; i++)
			sprintf(tmp + strlen(tmp), " %02x", p8[i]);
		break;

	case 2:
		sprintf(tmp, "%04x:", nline * bytes_per_line);
		for (i = 0; i < len / 2; i++)
			sprintf(tmp + strlen(tmp), " %04x", p16[i]);

		if (len % 2)	/* Last byte */
			sprintf(tmp + strlen(tmp), " %02x", p8[len - 1]);
		break;

	case 4:
		sprintf(tmp, "%04x:", nline * bytes_per_line);
		for (i = 0; i < len / 4; i++)
			sprintf(tmp + strlen(tmp), " %08x", p32[i]);

		for (i = len % 4; i > 0; i--)
			sprintf(tmp + strlen(tmp), " %02x", p8[len - i]);
		break;

	default:
		myprint("Error: not supported unit %d\n", unit);
		return;
	}

	myprint("%s\n", tmp);
}

void mydumpraw(void *buf, unsigned int len, unsigned int unit)
{
	unsigned int pos = 0, left = len, nline = 0;
	unsigned char *p = buf;

	if (unit != 1 && unit != 2 && unit != 4) {
		myprint("Error: Unsupported unit %d\n", unit);
		return;
	}

	do {
		if (left >= bytes_per_line) {
			print_line(nline, p + pos, bytes_per_line, unit);
			nline++;
			pos += bytes_per_line;
			left -= bytes_per_line;
		} else {
			print_line(nline, p + pos, left, unit);
			break;
		}
	} while (left);
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/* Example and output below */

static unsigned char data[128 + 7];
int main(void)
{

	int i;

	for (i = 0; i < sizeof(data); i++)
		data[i] = (i + 1) % 64;

	myprint("\n");
	mydumpraw(data, sizeof(data), 1);
	myprint("\n");
	mydumpraw(data, sizeof(data), 2);
	myprint("\n");
	mydumpraw(data, sizeof(data), 4);

	return 0;
}

/*
$ ./a

0000: 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10
0010: 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20
0020: 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30
0030: 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 00
0040: 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10
0050: 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20
0060: 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30
0070: 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 00
0080: 01 02 03 04 05 06 07

0000: 0201 0403 0605 0807 0a09 0c0b 0e0d 100f
0010: 1211 1413 1615 1817 1a19 1c1b 1e1d 201f
0020: 2221 2423 2625 2827 2a29 2c2b 2e2d 302f
0030: 3231 3433 3635 3837 3a39 3c3b 3e3d 003f
0040: 0201 0403 0605 0807 0a09 0c0b 0e0d 100f
0050: 1211 1413 1615 1817 1a19 1c1b 1e1d 201f
0060: 2221 2423 2625 2827 2a29 2c2b 2e2d 302f
0070: 3231 3433 3635 3837 3a39 3c3b 3e3d 003f
0080: 0201 0403 0605 07

0000: 04030201 08070605 0c0b0a09 100f0e0d
0010: 14131211 18171615 1c1b1a19 201f1e1d
0020: 24232221 28272625 2c2b2a29 302f2e2d
0030: 34333231 38373635 3c3b3a39 003f3e3d
0040: 04030201 08070605 0c0b0a09 100f0e0d
0050: 14131211 18171615 1c1b1a19 201f1e1d
0060: 24232221 28272625 2c2b2a29 302f2e2d
0070: 34333231 38373635 3c3b3a39 003f3e3d
0080: 04030201 05 06 07
*/
