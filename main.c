#include <stdio.h>
#include <stdint.h>

typedef struct BMPHEADER
{
  char Signature[2];
  int FileSize;
  int reserved;
  int DataOffset;
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
  char Blue;
  char Green;
  char Red;
  char Aplha; // Padding
} BGRA;

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
  
  BGRA color1;
  
  printf("%x\n",header.DataOffset);

  fseek(fptr,header.DataOffset,SEEK_SET);
  

  fread(&color1,sizeof(char),sizeof(BGRA),fptr);

  printf("%d %d %d %d\n",color1.Red,color1.Green,color1.Blue,color1.Aplha);

  return 0;
}
