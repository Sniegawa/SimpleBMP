#ifndef BMP_H
#define BMP_H

typedef struct bmp_image
{
  unsigned int width;
  unsigned int height;
  unsigned char* pixels; //RGB
}bmp_image;


bmp_image* bmp_load(const char* path);

void bmp_free(bmp_image* img);

#define BMP_IMPLEMENTATION // DELETEEEEEEEEE
#ifdef BMP_IMPLEMENTATION

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BMPHEADER_SIZE 14

typedef struct BMPHEADER
{
  uint8_t Signature[2];
  uint8_t FileSize[4];
  uint8_t reserved[4];
  uint8_t DataOffset[4];
}BMPHEADER;

typedef struct BMP_INFOHEADER
{
  int Size;
  int Width;
  int Height;
  char Planes[2];
  short int BPP;
  int Compression;
  int ImageSize; //Compressed size if Compression = 0 it should be = 0
  int XpixelsPerM;
  int YpixelsPerM;
  int ColorsUsed;
  int ImportantColors;
}BMP_INFOHEADER;


bmp_image* bmp_load(const char* path)
{
  bmp_image* img = (bmp_image*)malloc(sizeof(bmp_image));
  
  FILE* fptr;
  fptr = fopen(path,"rb");

  BMPHEADER header;

  size_t bytesRead = fread(&header, 1,BMPHEADER_SIZE,fptr);
  
  if(bytesRead != BMPHEADER_SIZE)
  {
    printf("Error reading file %s\nIncorrect header size\n",path);
    return NULL;
  }

  if(header.Signature[0] != 'B' || header.Signature[1] != 'M')
  {
    printf("Error reading file %s\nIncorrect header signature\n",path);
    return NULL;
  }

  BMP_INFOHEADER infoHeader;

  fseek(fptr,BMPHEADER_SIZE,SEEK_SET);

  bytesRead = fread(&infoHeader,1,sizeof(BMP_INFOHEADER),fptr);

  if(bytesRead != sizeof(BMP_INFOHEADER) || bytesRead != infoHeader.Size || infoHeader.Size != sizeof(BMP_INFOHEADER))
  {
    printf("Error reading file %s\nIncorrect info header size\n",path);
    return NULL;
  }

  img->width = infoHeader.Width;
  img->height = infoHeader.Height;

  int height = infoHeader.Height;
  int width = infoHeader.Width;
  
  if(infoHeader.Compression == 0) infoHeader.ImageSize = 0;

  if(infoHeader.BPP != 24)
  {
    printf("Error reading file %s\nUnsuported BPP value\n",path);
    return NULL;
  }

  int BytesPerPixel = (int)infoHeader.BPP/8;
  int RowBytes = width * BytesPerPixel;
  int PaddedRow = (RowBytes + 3) & ~3; // This should round up the value to multiple of 4
  int Padding = PaddedRow - RowBytes; 

  int DataSize = BytesPerPixel * width * height;
  int DataOffset = *(int*)header.DataOffset; // !!!!!! REFACTOR !!!!!!

  img->pixels = (unsigned char*)malloc(width * height * 3);

  unsigned char* row_buf = (unsigned char*)malloc(PaddedRow);

  fseek(fptr,DataOffset,SEEK_SET);

  for(unsigned int y = 0; y < height; ++y)
  {
    fread(row_buf,1,PaddedRow,fptr);
    
    int destination_y = height - 1 - y;
    unsigned char* dst_row = &img->pixels[destination_y * width * 3];

    for(unsigned int x = 0; x < width; ++x)
    {
      unsigned char b = row_buf[x * 3 + 0];
      unsigned char g = row_buf[x * 3 + 1];
      unsigned char r = row_buf[x * 3 + 2];

     dst_row[x * 3 + 0] = r;
     dst_row[x * 3 + 1] = g;
     dst_row[x * 3 + 2] = b;
    }

  }

  fclose(fptr);

  return img;
}

void bmp_free(bmp_image *img)
{
  if(!img) return;

  free(img->pixels);
  free(img);
}
#endif //BMP_IMPLEMENTATION
#endif //BMP_H

