#include <stdio.h>

#define BMP_IMPLEMENTATION
#include "BMP_READER.h"

int main(int argc, char** argv)
{
  bmp_image* img = bmp_load(argv[1]);
   
  printf("%d,%d\n",img->width,img->height);
  
  printf("%d,%d,%d\n",img->pixels[0],img->pixels[1],img->pixels[2]);
  free(img);
  return 0;
}
