#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
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

  printf("%d vs %d\n",(int)sizeof(BMPHEADER),(int)bytesRead);
  
  printf("%c%c\n",header.Signature[0],header.Signature[1]);

  BMP_INFOHEADER infoHeader;

  fseek(fptr, sizeof(BMPHEADER), SEEK_SET);
  size_t infoRead = fread(&infoHeader,sizeof(char),sizeof(BMP_INFOHEADER),fptr);

  printf("%d vs %d vs %d\n",(int)sizeof(BMP_INFOHEADER),(int)infoRead,infoHeader.Size);

  printf("%d %d\n",infoHeader.Width,infoHeader.Height);
  
  printf("Compression type = %d\n",infoHeader.Compression);
  if(infoHeader.Compression == 0) infoHeader.ImageSize = 0;
  printf("Bits per Pixel %d\n",infoHeader.BPP);

  long int NumColors;

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
  }
  
  BGRA colors[4];
  
  printf("%"PRIu32"\n",read_u32_from_arr(header.DataOffset));

  fseek(fptr,read_u32_from_arr(header.DataOffset),SEEK_SET);
  
  

  fread(&colors,sizeof(char),sizeof(BGRA)*4,fptr);

  printf("%d %d %d \n",colors[0].Red,colors[1].Red,colors[3].Red);

  return 0;
}
