/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FW2H_VERSION	"1.0.0.2"

void random_string16(char *str)
{
	const char letters[] = "0123456789ABCDEF";
	int i;

	srand((unsigned int)time(NULL));
	for (i = 0; i < sizeof(letters); ++i)
		str[i] = letters[rand() % 16];
	str[16] = '\0';
}

long filesize(FILE*stream)
{
	long curpos,length;

	curpos=ftell(stream);
	fseek(stream,0L,SEEK_END);
	length=ftell(stream);
	fseek(stream,curpos,SEEK_SET);

	return length;
}

int main(int argc, char *argv[])
{
	unsigned char read_byte;
	FILE *fp;
	int i, file_size;
	char guard_id[17];

    if (argc < 2) {
        fprintf(stderr, "FW2H v%s\n",FW2H_VERSION);
        fprintf(stderr, "no valid argument found\n");
        fprintf(stderr, "usage: fw2h <in-file> > <out-file>\n");
        exit(1);
    }
    
	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Open file failed.\n");
		exit(1);
	}

	file_size = filesize(fp);

	printf("/* This code was generated by fw2h utility, please don't make change to it directly.\n");
	printf(" * FW2H version: %s. */\n\n", FW2H_VERSION);
	random_string16(guard_id);
	printf("#ifndef BHY_FW_%s_H\n", guard_id);
	printf("#define BHY_FW_%s_H\n\n", guard_id);
	printf("const unsigned char bhy_firmware_image[%d] = {", file_size);

	for(i = 0; i < file_size; ++i) {
		if (i % 16 == 0)
			printf("\n\t");
		else
			printf(" ");

		fread(&read_byte, 1, 1, fp);

		printf("0x%02x,", read_byte);
	}

	fclose(fp);

	printf("\n};\n\n");
	printf("#endif /*~ BHY_FW_%s_H */\n", guard_id);

	exit(0);
}