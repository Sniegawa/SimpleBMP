#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct BGRA
{
  unsigned char Blue;
  unsigned char Green;
  unsigned char Red;
 // char Aplha; // Padding
} BGRA;

uint32_t read_u32_from_arr(const uint8_t b[4])
{
  uint32_t v;
  memcpy(&v, b, 4);
  return v;
}

int main(int argc, char** argv)
{
  FILE* fptr;
  fptr = fopen(argv[1],"rb");
  
  BMPHEADER header;

  size_t bytesRead = fread(&header, sizeof(char),sizeof(BMPHEADER),fptr);

  printf("sizeof BMP_HEADER  %d vs sizeof %d\n",(int)sizeof(BMPHEADER),(int)bytesRead);
  
  printf("Header signature : %c%c\n",header.Signature[0],header.Signature[1]);

  BMP_INFOHEADER infoHeader;

  fseek(fptr, sizeof(BMPHEADER), SEEK_SET);
  size_t infoRead = fread(&infoHeader,sizeof(char),sizeof(BMP_INFOHEADER),fptr);

  printf("sizeof BMP_INFOHEADER %d vs Bytes read %d vs size in read data%d\n",(int)sizeof(BMP_INFOHEADER),(int)infoRead,infoHeader.Size);

  printf("Read width %d : Read height %d\n",infoHeader.Width,infoHeader.Height);
  
  printf("Compression type = %d\n",infoHeader.Compression);
  if(infoHeader.Compression == 0) infoHeader.ImageSize = 0;
  printf("Bits per Pixel %d\n",infoHeader.BPP);

  /*long int NumColors;
  
  switch (infoHeader.BPP) 
  {
    case 1:
      NumColors = 1; break;
    case 4:
      NumColors = 16; break;
    case 8:
      NumColors = 256; break;
    case 16:
      NumColors = 65536; break;
    case 24:
      NumColors = 16 * 1024; break;
  }*/
  
  if (infoHeader.BPP != 24) return -1; // No support for other types of color storage
  
  int DataSize = ((int)infoHeader.BPP/8) * infoHeader.Width * infoHeader.Height;
  int DataOffset = *(int*)header.DataOffset;

  BGRA* ColorData = (BGRA*)malloc(DataSize);

  printf("DataSize : %d\nDataOffset %d\n",DataSize,DataOffset);

  fseek(fptr,DataOffset,SEEK_SET);
  
  size_t colorsRead = fread(ColorData,sizeof(char),(size_t)DataSize,fptr);

  printf("%d vs %u\n",(int)colorsRead,DataSize);
  BGRA color1 = ColorData[10];
  BGRA color2 = ColorData[500];
  printf("Color at idx 0 %d %d %d\n",color1.Red,color1.Green,color1.Blue);
  printf("Color at idx 150*150 %d %d %d\n",ColorData[150*150].Red,ColorData[150*150].Green,ColorData[150*150].Blue);

  return 0;
}
