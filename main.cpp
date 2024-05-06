
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <limits.h>

#include "aplib.h"


#ifndef CB_CALLCONV
# if defined(AP_DLL)
#  define CB_CALLCONV __stdcall
# elif defined(__GNUC__)
#  define CB_CALLCONV
# else
#  define CB_CALLCONV __cdecl
# endif
#endif


typedef unsigned char byte;

static unsigned int ratio(unsigned int x, unsigned int y)
{
	if (x <= UINT_MAX / 100) {
		x *= 100;
	}
	else {
		y /= 100;
	}

	if (y == 0) {
		y = 1;
	}

	return x / y;
}


int CB_CALLCONV callback(unsigned int insize, unsigned int inpos,
	unsigned int outpos, void* cbparam)
{
	(void)cbparam;

	printf("\rcompressed %u -> %u bytes (%u%% done)", inpos, outpos,
		ratio(inpos, insize));

	return 1;
}

static int compress_file(const char* oldname, const char* packedname)
{
	FILE* oldfile;
	FILE* packedfile;
	unsigned int insize, outsize;
	clock_t clocks;
	byte* data, * packed, * workmem;

	if ((oldfile = fopen(oldname, "rb")) == NULL) {
		printf("\nERR: unable to open input file\n");
		return 1;
	}

	fseek(oldfile, 0, SEEK_END);
	insize = (unsigned int)ftell(oldfile);
	fseek(oldfile, 0, SEEK_SET);

	if ((data = (byte*)malloc(insize)) == NULL ||
		(packed = (byte*)malloc(aP_max_packed_size(insize))) == NULL ||
		(workmem = (byte*)malloc(aP_workmem_size(insize))) == NULL) {
		printf("\nERR: not enough memory\n");
		return 1;
	}

	if (fread(data, 1, insize, oldfile) != insize) {
		printf("\nERR: error reading from input file\n");
		return 1;
	}

	clocks = clock();

	outsize = aPsafe_pack(data, packed, insize, workmem, callback, NULL);

	clocks = clock() - clocks;

	if (outsize == APLIB_ERROR) {
		printf("\nERR: an error occured while compressing\n");
		return 1;
	}

	if ((packedfile = fopen(packedname, "wb")) == NULL) {
		printf("\nERR: unable to create output file\n");
		return 1;
	}

	fwrite(packed, 1, outsize, packedfile);

	printf("\rCompressed %u -> %u bytes (%u%%) in %.2f seconds\n",
		insize, outsize, ratio(outsize, insize),
		(double)clocks / (double)CLOCKS_PER_SEC);

	fclose(packedfile);
	fclose(oldfile);

	free(workmem);
	free(packed);
	free(data);

	return 0;
}


static int decompress_file(const char* packedname, const char* newname)
{
	FILE* newfile;
	FILE* packedfile;
	unsigned int insize, outsize;
	clock_t clocks;
	byte* data, * packed;
	unsigned int depackedsize;

	if ((packedfile = fopen(packedname, "rb")) == NULL) {
		printf("\nERR: unable to open input file\n");
		return 1;
	}

	fseek(packedfile, 0, SEEK_END);
	insize = (unsigned int)ftell(packedfile);
	fseek(packedfile, 0, SEEK_SET);

	if ((packed = (byte*)malloc(insize)) == NULL) {
		printf("\nERR: not enough memory\n");
		return 1;
	}

	if (fread(packed, 1, insize, packedfile) != insize) {
		printf("\nERR: error reading from input file\n");
		return 1;
	}

	depackedsize = aPsafe_get_orig_size(packed);

	if (depackedsize == APLIB_ERROR) {
		printf("\nERR: compressed data error\n");
		return 1;
	}

	if ((data = (byte*)malloc(depackedsize)) == NULL) {
		printf("\nERR: not enough memory\n");
		return 1;
	}

	clocks = clock();

	outsize = aPsafe_depack(packed, insize, data, depackedsize);

	clocks = clock() - clocks;

	if (outsize != depackedsize) {
		printf("\nERR: an error occured while decompressing\n");
		return 1;
	}

	if ((newfile = fopen(newname, "wb")) == NULL) {
		printf("\nERR: unable to create output file\n");
		return 1;
	}

	fwrite(data, 1, outsize, newfile);

	printf("Decompressed %u -> %u bytes in %.2f seconds\n",
		insize, outsize,
		(double)clocks / (double)CLOCKS_PER_SEC);

	fclose(packedfile);
	fclose(newfile);

	free(packed);
	free(data);

	return 0;
}

static void show_syntax(void)
{
	printf("  Syntax:\n\n"
		"    Compress    :  appack c <file> <packed_file>\n"
		"    Decompress  :  appack d <packed_file> <depacked_file>\n\n");
}


int main(int argc, char* argv[])
{

	if (argc != 4) {
		show_syntax();
		return 1;
	}

#ifdef __WATCOMC__

	setbuf(stdout, NULL);
#endif

	if (argv[1][0] && argv[1][1] == '\0') {
		switch (argv[1][0]) {
		case 'c':
		case 'C': return compress_file(argv[2], argv[3]);

		case 'd':
		case 'D': return decompress_file(argv[2], argv[3]);
		}
	}

	show_syntax();
	return 1;
}
