/* rgbhex.c 
 *
 * Copyright (c) 2005, 2010 Victor Nilsson <victor@victornils.net>
 *
 * 2010-03-20 Fixed array size bugs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hexdigit(char c);
char *substr(char *src, int pos, int len, char *dest);
void set_rgbadd(int *rgb, int *rgbadd, int add, int perc);
void count_print(char *hexstr, int *rgbadd);

int main (int argc, char **argv)
{
	const char *usage = "Usage: rgbhex RRGGBB [-rgb] [INTEGER[%]]...\n"
		"Changes a hexadecimal RGB color value.\n"
		"\n"
		"Examples:\n"
		"  rgbhex B505F5 5           # Returns BA0AFA\n"
		"  rgbhex B505F5 -rg 7       # Returns BC0CF5\n"
		"  rgbhex B505F5 -g7 -rb-16  # Returns A50CE5\n";

	if (argc == 1 || !strcmp(argv[1], "--help")) {
		puts(usage);
		return 0;
	}

	int rgb[2];
	int rgbadd[] = { 0, 0, 0, 0, 0, 0 };
	char add_str[3];
	int add;
	int perc;
	int length;
	int i, j;

	for (i = 2; i < argc; i++) {
		rgb[0] = rgb[1] = rgb[2] = 0;
		perc = 0;

		if (argv[i][1] <= '9') {
			if (add != 1000)
				rgb[0] = rgb[1] = rgb[2] = 1;
			length = strlen(argv[i]);
			if (argv[i][length-1] == '%') {
				substr(argv[i], 0, -1, add_str);
				add = atoi(add_str);
				perc = 1;
			}
			else
				add = atoi(argv[i]);

			if (add == 0) {
				puts(usage);
				return 1;
			}
			set_rgbadd(rgb, rgbadd, add, perc);
			add = 0;
		}

		else {
			add = 1000;
			length = strlen(argv[i]);

			for (j = 1; j < length; j++) {
				if (add != 1000)
					break;

				switch (argv[i][j]) {
				case 'r':
					rgb[0] = 1;
					break;
				case 'g':
					rgb[1] = 1;
					break;
				case 'b':
					rgb[2] = 1;
					break;
				default:
					if (argv[i][length-1] == '%') {
						substr(argv[i], j, -1, add_str);
						perc = 1;
					}
					else
						substr(argv[i], j, 0, add_str);
					add = atoi(add_str);
					if (add == 0) {
						puts(usage);
						return 1;
					}
					set_rgbadd(rgb, rgbadd, add, perc);
				}
			}
		}
	}
	count_print(argv[1], rgbadd);
	return 0;
}

int hexdigit(char c)
{
	char ch[2];
	switch (c) {
	case 'A': case 'a': return 10;
	case 'B': case 'b': return 11;
	case 'C': case 'c': return 12;
	case 'D': case 'd': return 13;
	case 'E': case 'e': return 14;
	case 'F': case 'f': return 15;

	default:
		ch[0] = c;
		ch[1] = '\0';
		return atoi(ch);
	}
}

char *substr(char *src, int pos, int len, char *dst)
{
	int srclen;
	int i;

	srclen = strlen(src);
	if (srclen <= pos) {
		if (dst)
			dst = NULL;
		return NULL;
	}
	if (pos < 0) {
		pos = srclen + pos;
		if (pos < 0)
			pos = 0;
	}
	if (len > srclen)
		len = srclen - pos;
	else if (len < 1) {
		len = srclen - pos + len;
		if (len < 1) {
			if (!dst)
				dst = (char *) malloc(1);
			dst[0] = '\0';
			return dst;
		}
	}
	if (!dst)
		dst = (char *) malloc(len+1);
	for (i = 0; i < len; i++)
		dst[i] = src[pos+i];
	dst[i] = '\0';
	return dst;
}

void set_rgbadd(int *rgb, int *rgbadd, int add, int perc)
{
	int i;
	for (i = 0; i < 3; i++)
		if (rgb[i]) {
			if (perc)
				rgbadd[i*2+1] = add;
			else
				rgbadd[i*2] = add;
		}
	return;
}

void count_print(char *hexstr, int *rgbadd)
{
	char *hexchr = "0123456789ABCDEF";
	int rgb[3];
	int i;

	for (i = 0; i < 3; i++) {
		rgb[i] = hexdigit(hexstr[i*2])*16 + hexdigit(hexstr[i*2+1]);

		if (rgbadd[i*2])
			rgb[i] += rgbadd[i*2];
		if (rgbadd[i*2+1])
			rgb[i] += (int) (rgb[i] * rgbadd[i*2+1] / (float) 100);

		if (rgb[i] > 255)
			rgb[i] = 255;
		else if (rgb[i] < 0)
			rgb[i] = 0;

		printf("%c%c", hexchr[rgb[i]/16], hexchr[rgb[i]%16]);
	}
	printf("\n");
	return;
}
