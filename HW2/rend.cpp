#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    <string>

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

#define RGBA_DIMEMSION	4	/* RGBA -> 4D color */
#define RGB_DIMEMSION	3	/* RGB -> 3D color */
#define COLOR_LIMIT		4095	/* Clamping the color value into 0~4095. */

GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;

	int resolution = 0;
	resolution = xres * yres;
	int frameBufferDepth = (RGB_DIMEMSION + 1) * resolution;	// Add 1 as the Z 
	framebuffer = new char[frameBufferDepth];
	pixelbuffer = new GzPixel[resolution];
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel defaultPixel = { 880, 880, 880, 1, MAXINT };

	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		pixelbuffer[i] = defaultPixel;
		framebuffer[RGB_DIMEMSION * i] = (char)880;
		framebuffer[RGB_DIMEMSION * i + 1] = (char)880;
		framebuffer[RGB_DIMEMSION * i + 2] = (char)880;
		framebuffer[RGB_DIMEMSION * i + 3] = (char)MAXINT;	// initialize Z.
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		GzPixel currentPixel = { r, g, b, a, z };
		pixelbuffer[index] = currentPixel;
	}
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		GzPixel currentPixel = pixelbuffer[index];
		*r = currentPixel.red;
		*g = currentPixel.green;
		*b = currentPixel.blue;
		*a = currentPixel.alpha;
		*z = currentPixel.z;
	}
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);
	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		GzPixel currentPixel = pixelbuffer[i];

		GzIntensity gotRed = currentPixel.red;
		GzIntensity gotGreen = currentPixel.green;
		GzIntensity gotBlue = currentPixel.blue;
		if (currentPixel.red < 0)
			gotRed = 0;
		if (currentPixel.red > COLOR_LIMIT)
			gotRed = COLOR_LIMIT;
		if (currentPixel.green < 0)
			gotGreen = 0;
		if (currentPixel.green > COLOR_LIMIT)
			gotGreen = COLOR_LIMIT;
		if (currentPixel.blue < 0)
			gotBlue = 0;
		if (currentPixel.blue > COLOR_LIMIT)
			gotBlue = COLOR_LIMIT;

		GzIntensity red2 = gotRed >> 4;
		char redValue = (char)(red2 & 0xFF);
		GzIntensity green2 = gotGreen >> 4;
		char greenValue = (char)(green2 & 0xFF);
		GzIntensity blue2 = gotBlue >> 4;
		char blueValue = (char)(blue2 & 0xFF);

		char color[3];
		color[0] = redValue;
		color[1] = greenValue;
		color[2] = blueValue;
		fwrite(color, 1, 3, outfile);
	}
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		GzPixel currentPixel = pixelbuffer[i];

		GzIntensity gotRed = currentPixel.red;
		GzIntensity gotGreen = currentPixel.green;
		GzIntensity gotBlue = currentPixel.blue;
		if (currentPixel.red < 0)
			gotRed = 0;
		if (currentPixel.red > COLOR_LIMIT)
			gotRed = COLOR_LIMIT;
		if (currentPixel.green < 0)
			gotGreen = 0;
		if (currentPixel.green > COLOR_LIMIT)
			gotGreen = COLOR_LIMIT;
		if (currentPixel.blue < 0)
			gotBlue = 0;
		if (currentPixel.blue > COLOR_LIMIT)
			gotBlue = COLOR_LIMIT;

		GzIntensity red2 = gotRed >> 4;
		char redValue = (char)(red2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 2] = redValue;

		GzIntensity green2 = gotGreen >> 4;
		char greenValue = (char)(green2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 1] = greenValue;

		GzIntensity blue2 = gotBlue >> 4;
		char blueValue = (char)(blue2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i] = blueValue;
	}
	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/
	if (numAttributes == 1) {
		GzToken token = nameList[0];
		float* color = (float*)valueList[0];
		flatcolor[0] = color[0];
		flatcolor[1] = color[1];
		flatcolor[2] = color[2];
		//GzColor* color1 = (GzColor*)valueList[1];
		//flatcolor[1] = *color[1];
		//GzColor* color2 = (GzColor*)valueList[2];
		//flatcolor[2] = *color[2];
		//float* color1 = (float*) *valueList;
		//flatcolor[0] = *color1;
		//printf("%d. \n", sizeof(token));
		//valueList += token;
		//float* color2 = (float*) *valueList;
		//flatcolor[1] = *color2;
		//valueList += token;
		//float* color3 = (float*) *valueList;
		//flatcolor[2] = *color3;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken *nameList, GzPointer *valueList) 
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	if (numParts == 1) {
		GzCoord* verticesPointer = (GzCoord*)valueList[0];
		GzCoord vertices[3], sortedVertices[3];
		// V1:
		vertices[0][0] = verticesPointer[0][0];
		vertices[0][1] = verticesPointer[0][1];
		vertices[0][2] = verticesPointer[0][2];
		// V2:
		vertices[1][0] = verticesPointer[1][0];
		vertices[1][1] = verticesPointer[1][1];
		vertices[1][2] = verticesPointer[1][2];
		// V3:
		vertices[2][0] = verticesPointer[2][0];
		vertices[2][1] = verticesPointer[2][1];
		vertices[2][2] = verticesPointer[2][2];

		if (vertices[0][1] > vertices[1][1]) {
			float tempX, tempY, tempZ;
			tempX = vertices[0][0];
			tempY = vertices[0][1];
			tempZ = vertices[0][2];
			vertices[0][0] = vertices[1][0];
			vertices[0][1] = vertices[1][1];
			vertices[0][2] = vertices[1][2];
			vertices[1][0] = tempX;
			vertices[1][1] = tempY;
			vertices[1][2] = tempZ;
		}
		if (vertices[0][1] > vertices[2][1]) {
			float tempX, tempY, tempZ;
			tempX = vertices[0][0];
			tempY = vertices[0][1];
			tempZ = vertices[0][2];
			vertices[0][0] = vertices[2][0];
			vertices[0][1] = vertices[2][1];
			vertices[0][2] = vertices[2][2];
			vertices[2][0] = tempX;
			vertices[2][1] = tempY;
			vertices[2][2] = tempZ;
		}
		if (vertices[1][1] > vertices[2][1]) {
			float tempX, tempY, tempZ;
			tempX = vertices[1][0];
			tempY = vertices[1][1];
			tempZ = vertices[1][2];
			vertices[1][0] = vertices[2][0];
			vertices[1][1] = vertices[2][1];
			vertices[1][2] = vertices[2][2];
			vertices[2][0] = tempX;
			vertices[2][1] = tempY;
			vertices[2][2] = tempZ;
		}
		//sorted by Y. determine final order by middle-Y & special cases.
		if (vertices[0][1] == vertices[1][1]) {
			if (vertices[0][0] > vertices[1][0]) {
				float tempX, tempY, tempZ;
				tempX = vertices[1][0];
				tempY = vertices[1][1];
				tempZ = vertices[1][2];
				vertices[1][0] = vertices[2][0];
				vertices[1][1] = vertices[2][1];
				vertices[1][2] = vertices[2][2];
				vertices[2][0] = tempX;
				vertices[2][1] = tempY;
				vertices[2][2] = tempZ;
			}
		}
		else if (vertices[1][1] == vertices[2][1]) {
			if (vertices[2][0] > vertices[1][0]) {
				float tempX, tempY, tempZ;
				tempX = vertices[1][0];
				tempY = vertices[1][1];
				tempZ = vertices[1][2];
				vertices[1][0] = vertices[2][0];
				vertices[1][1] = vertices[2][1];
				vertices[1][2] = vertices[2][2];
				vertices[2][0] = tempX;
				vertices[2][1] = tempY;
				vertices[2][2] = tempZ;
			}
		}
		else {
			float slopeOfSideEdge = (vertices[0][1] - vertices[2][1]) / (vertices[0][0] - vertices[2][0]);
			float middleX = (vertices[1][1] - vertices[0][1]) / slopeOfSideEdge + vertices[0][0];

			if (middleX > vertices[1][0]) {
				float tempX, tempY, tempZ;
				tempX = vertices[1][0];
				tempY = vertices[1][1];
				tempZ = vertices[1][2];
				vertices[1][0] = vertices[2][0];
				vertices[1][1] = vertices[2][1];
				vertices[1][2] = vertices[2][2];
				vertices[2][0] = tempX;
				vertices[2][1] = tempY;
				vertices[2][2] = tempZ;
			}
		}
		//sorted as CW. 3 edges: 1-2, 2-3, 3-1.
		float slope12, slope23, slope31;

		




		
		
	}
	return GZ_SUCCESS;
}

