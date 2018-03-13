/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */

  //bound check
  //if(u<0) 
  float tv = v;
  float tu = u;
  if (tv < 0) tv = 0.0;
  if (tv > 1) tv = 1.0;
  if (tu < 0) tu = 0.0;
  if (tu > 1) tu = 1.0;

  //Scale u,v range to x,y texture image size (x-size, y-size)
  float tx = tu * (xs - 1);
  float ty = tv * (ys - 1);
  //nearest-neighbor
  int x1 = floor(tx), x2 = ceil(tx), x3 = ceil(tx), x4 = floor(tx);
  int y1 = floor(ty), y2 = floor(ty), y3 = ceil(ty), y4 = ceil(ty);

  //  Interpolate nearest - neighbor texel colors to scaled(x, y)
  int index1 = x1 + y1 * xs;
  int index2 = x2 + y2 * xs;
  int index3 = x3 + y3 * xs;
  int index4 = x4 + y4 * xs;

  //Color(p) = s t C + (1-s) t D + s (1-t) B + (1-s) (1-t) A
  color[0] = (1 - tu)*(1 - tv)*image[index1][0] + tu*(1 - tv)*image[index2][0] + tv * tu * image[index3][0] + (1 - tu)*tv*image[index4][0];
  color[1] = (1 - tu)*(1 - tv)*image[index1][1] + tu*(1 - tv)*image[index2][1] + tv * tu * image[index3][1] + (1 - tu)*tv*image[index4][1];
  color[2] = (1 - tu)*(1 - tv)*image[index1][2] + tu*(1 - tv)*image[index2][2] + tv * tu * image[index3][2] + (1 - tu)*tv*image[index4][2];

  return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	float tv = v;
	float tu = u;
	if (tv < 0) tv = 0.0;
	if (tv > 1) tv = 1.0;
	if (tu < 0) tu = 0.0;
	if (tu > 1) tu = 1.0;

	int px = tv * 300;
	int py = tu * 300;
	if (px / 100 == 0) {
		color[RED] = 1;
		color[GREEN] = 0;
		color[BLUE] = 0;
	}
	else if (px /100 == 1) {
		color[RED] = 0;
		color[GREEN] = 1;
		color[BLUE] = 0;
	}
	else {
		color[RED] = 0;
		color[GREEN] = 0;
		color[BLUE] = 1;
	}


	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

