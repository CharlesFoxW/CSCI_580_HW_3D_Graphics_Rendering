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
  
  if (u > 1.0f)
	  u = 1.0f;
  if (u < 0)
	  u = 0;
  if (v > 1.0f)
	  v = 1.0f;
  if (v < 0)
	  v = 0;
  
  float factorU = (float)(xs - 1);
  float factorV = (float)(ys - 1);
  float rawU = u * factorU;
  float rawV = v * factorV;



  int upperBoundX = (int)ceil(rawU);
  int lowerBoundX = (int)floor(rawU);
  int upperBoundY = (int)ceil(rawV);
  int lowerBoundY = (int)floor(rawV);

  float the_s = rawU - (float)lowerBoundX;
  float the_t = rawV - (float)lowerBoundY;

  GzColor colorA, colorB, colorC, colorD;

  colorA[RED] = image[lowerBoundY * xs + lowerBoundX][RED];
  colorA[GREEN] = image[lowerBoundY * xs + lowerBoundX][GREEN];
  colorA[BLUE] = image[lowerBoundY * xs + lowerBoundX][BLUE];

  colorB[RED] = image[lowerBoundY * xs + upperBoundX][RED];
  colorB[GREEN] = image[lowerBoundY * xs + upperBoundX][GREEN];
  colorB[BLUE] = image[lowerBoundY * xs + upperBoundX][BLUE];

  colorC[RED] = image[upperBoundY * xs + upperBoundX][RED];
  colorC[GREEN] = image[upperBoundY * xs + upperBoundX][GREEN];
  colorC[BLUE] = image[upperBoundY * xs + upperBoundX][BLUE];

  colorD[RED] = image[upperBoundY * xs + lowerBoundX][RED];
  colorD[GREEN] = image[upperBoundY * xs + lowerBoundX][GREEN];
  colorD[BLUE] = image[upperBoundY * xs + lowerBoundX][BLUE];
  /*
  char buffer[50];
  sprintf(buffer, "tex_fun = %4.4f, %4.4f", rawU, rawV);
  OutputDebugStringA(buffer);
  */
  color[RED] = the_s * the_t * colorC[RED] + (1.0f - the_s) * the_t * colorD[RED]
	  + the_s * (1.0f - the_t) * colorB[RED] + (1.0f - the_s) * (1.0f - the_t) * colorA[RED];
  color[GREEN] = the_s * the_t * colorC[GREEN] + (1 - the_s) * the_t * colorD[GREEN]
	  + the_s * (1.0f - the_t) * colorB[GREEN] + (1.0f - the_s) * (1.0f - the_t) * colorA[GREEN];
  color[BLUE] = the_s * the_t * colorC[BLUE] + (1.0f - the_s) * the_t * colorD[BLUE]
	  + the_s * (1.0f - the_t) * colorB[BLUE] + (1.0f - the_s) * (1.0f - the_t) * colorA[BLUE];


  
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	// Checkboard of "Blue and GreyBlue"
	xs = 100;
	ys = 100;

	int quadSize = 20;

	if (u > 1.0f)
		u = 1.0f;
	if (u < 0)
		u = 0;
	if (v > 1.0f)
		v = 1.0f;
	if (v < 0)
		v = 0;

	float factorU = (float)(xs - 1);
	float factorV = (float)(ys - 1);
	int rawU = (int)round(u * factorU);
	int rawV = (int)round(v * factorV);

	if (rawU % quadSize < quadSize / 2 && rawV % quadSize < quadSize / 2) {
		color[RED] = 0.5f;
		color[GREEN] = 0.9f;
		color[BLUE] = 0.9f;
	}
	else if (rawU % quadSize >= quadSize / 2 && rawV % quadSize >= quadSize / 2) {
		color[RED] = 0.5f;
		color[GREEN] = 0.9f;
		color[BLUE] = 0.9f;
	}
	else {
		color[RED] = 0.2f;
		color[GREEN] = 0.4f;
		color[BLUE] = 0.8f;
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

