/*
	Œniegawa - 2025

	To use this library you simply have to paste this:

	#define BMP_IMPLEMENTATION
	#include "SimpleBMP.h"

	Library as of today only supports 24-Bit maps.

*/

#ifndef BMP_H
#define BMP_H

typedef struct BMP_IMAGE
{
  unsigned int width;
  unsigned int height;
  unsigned char* pixels; //RGB
}BMP_IMAGE;


BMP_IMAGE* BMP_LOAD(const char* path);

void BMP_FREE(BMP_IMAGE* img);

void BMP_WRITE(BMP_IMAGE* img, const char* path);

#ifdef BMP_IMPLEMENTATION

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE 14
#define INFOHEADER_SIZE 40

typedef struct BMP_DATA
{
	const unsigned char* data;
	size_t size;
	size_t pos;
}BMP_DATA;

static void BMP_SKIP_BYTES(BMP_DATA* d, int n)
{
	if (d->pos + n > d->size)
		n = d->size - d->pos;
	d->pos += n;
}

static unsigned char BMP_GET8(BMP_DATA* d)
{
	if (d->pos > d->size) return 0;
	return d->data[d->pos++]; // Get data at current pos and advance further
}

//Little endian
static uint32_t BMP_GET32(BMP_DATA* d)
{
	uint32_t v = 0;
	v |= BMP_GET8(d);
	v |= BMP_GET8(d) << 8; // Get next byte and shift it to the right by one byte (It retrieves value as uint8_t but it's promoted to uint32_t value on operations so it doesn't oferflow)
	v |= BMP_GET8(d) << 16;
	v |= BMP_GET8(d) << 24;
	return v;
}

//Little endian
static uint16_t BMP_GET16(BMP_DATA* d)
{
	uint16_t v = 0;
	v |= BMP_GET8(d);
	v |= BMP_GET8(d) << 8;
	return v;
}

BMP_IMAGE* BMP_LOAD(const char* path)
{
	BMP_IMAGE* img = (BMP_IMAGE*)malloc(sizeof(BMP_IMAGE));

	FILE* fptr;
	fptr = fopen(path, "rb");

	if (!fptr)
	{
		printf("BMP : Error opening file %s for reading\n", path);
		return NULL;
	}

	fseek(fptr, 0, SEEK_END); // Go to end of file
	size_t size = ftell(fptr); // Get file size by retrieving the FILE's current cursor position
	fseek(fptr, 0, SEEK_SET); // Return back to the beggining to file

	unsigned char* fileData = malloc(size);
	fread(fileData, 1, size, fptr);
	fclose(fptr);

	BMP_DATA d = { fileData, size, 0 };

	if(BMP_GET8(&d) != 'B' || BMP_GET8(&d) != 'M')
	{
		free(fileData);
		printf("BMP : Corrupted header data\n");
		return NULL;
	}

	BMP_SKIP_BYTES(&d, 8); // Skipping File size, since we have it and reserved data space

	uint32_t PixelDataOffset = BMP_GET32(&d);

	uint32_t header_size = BMP_GET32(&d);

	int width = (int)BMP_GET32(&d);
	int height = (int)BMP_GET32(&d);

	img->width = width;
	img->height = height;

	BMP_SKIP_BYTES(&d, 2); // Planes

	uint16_t bpp = BMP_GET16(&d); // Bits per pixel

	if(bpp != 24)
	{
		free(fileData);
		printf("BMP : unsuported BMP bit format (only 24-bit supported)\n");
		return NULL;
	}

	BMP_SKIP_BYTES(&d, 24); // Skip rest of info header

	//----------READING DATA-------------\\

	img->pixels = (unsigned char*)malloc((size_t)width * height * 3);

	d.pos = PixelDataOffset;

	int row_bytes = width * 3;
	int pad = (4 - (row_bytes & 3)) & 3;

	for (int y = 0; y < height; ++y)
	{
		size_t row = height - 1 - y; // Flip
		size_t z = (size_t)row * (size_t)width * 3;

		for (int x = 0; x < width; ++x)
		{
			unsigned char b = BMP_GET8(&d);
			unsigned char g = BMP_GET8(&d);
			unsigned char r = BMP_GET8(&d);
			
			img->pixels[z++] = r;
			img->pixels[z++] = g;
			img->pixels[z++] = b;
		}

		BMP_SKIP_BYTES(&d, pad);

	}
	free(fileData);

	return img;
}

void BMP_FREE(BMP_IMAGE*img)
{
	if(!img) return;

	free(img->pixels);
	free(img);
}

void BMP_WRITE(BMP_IMAGE* img, const char* path)
{
	FILE* fptr = fopen(path, "wb");

	if(!fptr)
	{
		printf("BMP : Error opening file %s for writing\n", path);
		fclose(fptr);
		return;
	}

	const unsigned char* Signature = "BM";
	fwrite(Signature, 1, 2, fptr);


	int width = img->width;
	int height = img->height;

	int rowSize = ((width * 3 + 3) / 4) * 4;

	unsigned int FileSize = rowSize * height + HEADER_SIZE + INFOHEADER_SIZE;

	fwrite(&FileSize, 4, 1, fptr);
	
	for (int i = 0; i < 4; ++i)
		putc(0, fptr);

	int DataOffset = HEADER_SIZE + INFOHEADER_SIZE;
	fwrite(&DataOffset, 4, 1, fptr);

	//INFO HEADER

	int ihSize = INFOHEADER_SIZE;
	fwrite(&ihSize, 4, 1, fptr);

	fwrite(&img->width, 4, 1, fptr);
	fwrite(&img->height, 4, 1, fptr);

	short int Planes = 1;

	fwrite(&Planes, 2, 1, fptr);

	short int BitCount = 24; // FOR NOW ONLY SUPPORT FOR 24 BIT
	fwrite(&BitCount, 2, 1, fptr);

	int Compression = 0;
	fwrite(&Compression, 4, 1, fptr);

	unsigned int zeros[5] = { 0,0,0,0,0 };
	fwrite(zeros, 4, 5, fptr);

	unsigned char* bgrPixels = (unsigned char*)malloc(width * height * 3);

	for(size_t i = 0; i < width*height; ++i)
	{
		bgrPixels[i * 3 + 0] = img->pixels[i * 3 + 2];
		bgrPixels[i * 3 + 1] = img->pixels[i * 3 + 1];
		bgrPixels[i * 3 + 2] = img->pixels[i * 3 + 0];
	}

	int padding = rowSize - width*3;

	unsigned char pad[3] = { 0,0,0 };
	for(int y = height-1; y >= 0; y--)
	{
		for(int x = 0; x < width; x++)
		{
			unsigned char r = bgrPixels[(y * width + x) * 3 + 0];
			unsigned char g = bgrPixels[(y * width + x) * 3 + 1];
			unsigned char b = bgrPixels[(y * width + x) * 3 + 2];
			unsigned char pixel[3] = { r,g,b };
			fwrite(pixel, 1, 3, fptr);
		}

		if (padding > 0) fwrite(pad, 1, padding, fptr);

	}

	fclose(fptr);
}


#endif //BMP_IMPLEMENTATION
#endif //BMP_H

