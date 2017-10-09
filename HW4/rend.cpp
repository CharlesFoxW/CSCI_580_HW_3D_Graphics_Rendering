/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

#define RGBA_DIMEMSION	4	/* RGBA -> 4D color */
#define RGB_DIMEMSION	3	/* RGB -> 3D color */
#define COLOR_LIMIT		4095	/* Clamping the color value into 0~4095. */

static short matlevelNormal;	/* Global variable of the head of matrix stack of normals */
int pushMatToStack(short &head, GzMatrix* matStack, GzMatrix mat);
int popMatToStack(short &head, GzMatrix* matStack);

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	float radianAngle = degree * PI / 180.0f;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = 1.0;
	mat[1][1] = (float)cos((double)radianAngle);
	mat[1][2] = -1.0f * (float)sin((double)radianAngle);
	mat[2][1] = (float)sin((double)radianAngle);
	mat[2][2] = (float)cos((double)radianAngle);
	mat[3][3] = 1.0;

	if (pushMatToStack(matlevelNormal, Xnorm, mat))
		return GZ_FAILURE;
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	float radianAngle = degree * PI / 180.0f;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = (float)cos((double)radianAngle);
	mat[0][2] = (float)sin((double)radianAngle);
	mat[1][1] = 1.0;
	mat[2][0] = -1.0f * (float)sin((double)radianAngle);
	mat[2][2] = (float)cos((double)radianAngle);
	mat[3][3] = 1.0;

	if (pushMatToStack(matlevelNormal, Xnorm, mat))
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	float radianAngle = degree * PI / 180.0f;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = (float)cos((double)radianAngle);
	mat[0][1] = -1.0f * (float)sin((double)radianAngle);
	mat[1][0] = (float)sin((double)radianAngle);
	mat[1][1] = (float)cos((double)radianAngle);
	mat[2][2] = 1.0;
	mat[3][3] = 1.0;

	if (pushMatToStack(matlevelNormal, Xnorm, mat))
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = 1.0;
	mat[0][3] = translate[0];
	mat[1][1] = 1.0;
	mat[1][3] = translate[1];
	mat[2][2] = 1.0;
	mat[2][3] = translate[2];
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			mat[i][j] = 0;
		}
	}
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


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
	int frameBufferDepth = (RGB_DIMEMSION)* resolution;	// Add 1 as the Z 
	framebuffer = new char[frameBufferDepth];
	pixelbuffer = new GzPixel[resolution];

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	matlevel = -1;
	matlevelNormal = -1;

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			Xsp[i][j] = 0;
		}
	}
	Xsp[0][0] = (float)xres / 2.0f;
	Xsp[0][3] = (float)xres / 2.0f;
	Xsp[1][1] = -1.0f * (float)yres / 2.0f;
	Xsp[1][3] = (float)yres / 2.0f;
	Xsp[2][2] = (float)MAXINT;
	Xsp[3][3] = 1.0;

	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;
	for (int i = 0; i < 3; i++) {
		m_camera.lookat[i] = 0;
		m_camera.worldup[i] = 0;
	}
	m_camera.worldup[1] = 1.0;
	m_camera.FOV = DEFAULT_FOV;

	numlights = 0;
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
	GzPixel defaultPixel = { 2880, 2880, 2880, 1, MAXINT };

	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		pixelbuffer[i] = defaultPixel;
		framebuffer[RGB_DIMEMSION * i] = (char)2880;
		framebuffer[RGB_DIMEMSION * i + 1] = (char)2880;
		framebuffer[RGB_DIMEMSION * i + 2] = (char)2880;
		//framebuffer[RGB_DIMEMSION * i + 3] = (char)MAXINT;	// initialize Z.
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 

// Define I:
	GzMatrix matrix_I;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			matrix_I[j][i] = 0;
		}
	}
	matrix_I[0][0] = 1;
	matrix_I[1][1] = 1;
	matrix_I[2][2] = 1;
	matrix_I[3][3] = 1;

// Compute Xiw:
	GzCoord cl, newUp, the_X, the_Y, the_Z;
	for (int i = 0; i < 3; i++) {
		cl[i] = m_camera.lookat[i] - m_camera.position[i];
	}
	the_Z[0] = cl[0] / (float)sqrt((double)(cl[0] * cl[0] + cl[1] * cl[1] + cl[2] * cl[2]));
	the_Z[1] = cl[1] / (float)sqrt((double)(cl[0] * cl[0] + cl[1] * cl[1] + cl[2] * cl[2]));
	the_Z[2] = cl[2] / (float)sqrt((double)(cl[0] * cl[0] + cl[1] * cl[1] + cl[2] * cl[2]));

	float upDotZ = m_camera.worldup[0] * the_Z[0] + m_camera.worldup[1] * the_Z[1] + m_camera.worldup[2] * the_Z[2];
	newUp[0] = m_camera.worldup[0] - upDotZ * the_Z[0];
	newUp[1] = m_camera.worldup[1] - upDotZ * the_Z[1];
	newUp[2] = m_camera.worldup[2] - upDotZ * the_Z[2];
	the_Y[0] = newUp[0] / (float)sqrt((double)(newUp[0] * newUp[0] + newUp[1] * newUp[1] + newUp[2] * newUp[2]));
	the_Y[1] = newUp[1] / (float)sqrt((double)(newUp[0] * newUp[0] + newUp[1] * newUp[1] + newUp[2] * newUp[2]));
	the_Y[2] = newUp[2] / (float)sqrt((double)(newUp[0] * newUp[0] + newUp[1] * newUp[1] + newUp[2] * newUp[2]));

	the_X[0] = the_Y[1] * the_Z[2] - the_Y[2] * the_Z[1];
	the_X[1] = the_Y[2] * the_Z[0] - the_Y[0] * the_Z[2];
	the_X[2] = the_Y[0] * the_Z[1] - the_Y[1] * the_Z[0];

	m_camera.Xiw[0][0] = the_X[0];
	m_camera.Xiw[0][1] = the_X[1];
	m_camera.Xiw[0][2] = the_X[2];
	m_camera.Xiw[1][0] = the_Y[0];
	m_camera.Xiw[1][1] = the_Y[1];
	m_camera.Xiw[1][2] = the_Y[2];
	m_camera.Xiw[2][0] = the_Z[0];
	m_camera.Xiw[2][1] = the_Z[1];
	m_camera.Xiw[2][2] = the_Z[2];
	m_camera.Xiw[0][3] = -1.0f * (the_X[0] * m_camera.position[0] + the_X[1] * m_camera.position[1] + the_X[2] * m_camera.position[2]);
	m_camera.Xiw[1][3] = -1.0f * (the_Y[0] * m_camera.position[0] + the_Y[1] * m_camera.position[1] + the_Y[2] * m_camera.position[2]);
	m_camera.Xiw[2][3] = -1.0f * (the_Z[0] * m_camera.position[0] + the_Z[1] * m_camera.position[1] + the_Z[2] * m_camera.position[2]);
	m_camera.Xiw[3][0] = 0;
	m_camera.Xiw[3][1] = 0;
	m_camera.Xiw[3][2] = 0;
	m_camera.Xiw[3][3] = 1;

	// Compute Xpi:
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			m_camera.Xpi[i][j] = 0;
		}
	}
	m_camera.Xpi[0][0] = 1;
	m_camera.Xpi[1][1] = 1;
	m_camera.Xpi[2][2] = (float)tan((double)(m_camera.FOV * PI / 180.0) / 2.0);
	m_camera.Xpi[3][2] = (float)tan((double)(m_camera.FOV * PI / 180.0) / 2.0);
	m_camera.Xpi[3][3] = 1;

	// Push Xsp:
	int status = 0;
	status |= GzPushMatrix(Xsp);
	status |= GzPushMatrix(m_camera.Xpi);
	status |= GzPushMatrix(m_camera.Xiw);

	status |= pushMatToStack(matlevelNormal, Xnorm, matrix_I);
	status |= pushMatToStack(matlevelNormal, Xnorm, matrix_I);

	GzMatrix XiwNormal;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			XiwNormal[j][i] = m_camera.Xiw[j][i];
		}
	}
	XiwNormal[0][3] = 0;
	XiwNormal[1][3] = 0;
	XiwNormal[2][3] = 0;

	status |= pushMatToStack(matlevelNormal, Xnorm, XiwNormal);

	if (status)
		return GZ_FAILURE;
	else
		return GZ_SUCCESS;

}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			m_camera.Xiw[i][j] = camera.Xiw[i][j];
			m_camera.Xpi[i][j] = camera.Xpi[i][j];
		}
	}

	for (int i = 0; i < 3; i++) {
		m_camera.position[i] = camera.position[i];
		m_camera.lookat[i] = camera.lookat[i];
		m_camera.worldup[i] = camera.worldup[i];
	}
	m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;	
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
/* HW 3.9 
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	/*
	if (matlevel < MATLEVELS) {
		if (matlevel == -1) {
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					Ximage[0][i][j] = matrix[i][j];
				}
			}
		}
		else {
			// Matrix Multiplication:
			for (int k = 0; k < 4; k++) {
				for (int j = 0; j < 4; j++) {
					float sum = 0;
					for (int i = 0; i < 4; i++) {
						sum += Ximage[matlevel][k][i] * matrix[i][j];
					}
					Ximage[matlevel + 1][k][j] = sum;
				}
			}
		}
		// increment matlevel
		matlevel++;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
	*/
	if (pushMatToStack(matlevel, Ximage, matrix))
		return GZ_FAILURE;
	return GZ_SUCCESS;

}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	
	if(popMatToStack(matlevel, Ximage))
		return GZ_FAILURE;
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		if (z < pixelbuffer[index].z) {
			GzPixel currentPixel = { r, g, b, a, z };
			pixelbuffer[index] = currentPixel;
		}
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
		GzDepth gotZ = currentPixel.z;
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

		//framebuffer[RGB_DIMEMSION * i + 3] = (char)gotZ;
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
	/*
	if (numAttributes == 1) {
		GzToken token = nameList[0];
		float* color = (float*)valueList[0];
		flatcolor[0] = color[0];
		flatcolor[1] = color[1];
		flatcolor[2] = color[2];
	}
	*/
	/*
	int currentIndex = numAttributes;
	//float* light = (float*)valueList[0];
	valueList += sizeof(GzLight*) / sizeof(char*);
	GzLight* light = (GzLight*) *valueList;
	//GzColor c;
	//c[0] = light->color[0];
	flatcolor[0] = light->color[0];
	flatcolor[1] = light->color[1];
	flatcolor[2] = light->color[2];
	
	
	char buffer[50];
	sprintf_s(buffer, "Light Color: %4.2f, %4.2f, %4.2f, \n", flatcolor[0], flatcolor[1], flatcolor[2]);
	OutputDebugStringA(buffer);
	*/

	int index = 0;
	while (index < numAttributes) {
		GzToken token = nameList[index];

		switch (token) {
		case GZ_RGB_COLOR: {
			GzColor* colorRGB = (GzColor*)valueList[index];
			flatcolor[0] = (*colorRGB)[0];
			flatcolor[1] = (*colorRGB)[1];
			flatcolor[2] = (*colorRGB)[2];
		}
		break;
		case GZ_INTERPOLATE: {
			int* mode = (int*)valueList[index];
			interp_mode = *mode;
		}
		break;
		case GZ_DIRECTIONAL_LIGHT: {
			GzLight* lightDir = (GzLight*)valueList[index];
			lights[numlights].direction[0] = lightDir->direction[0];
			lights[numlights].direction[1] = lightDir->direction[1];
			lights[numlights].direction[2] = lightDir->direction[2];
			lights[numlights].color[0] = lightDir->color[0];
			lights[numlights].color[1] = lightDir->color[1];
			lights[numlights].color[2] = lightDir->color[2];
			numlights++;
		}
		break;
		case GZ_AMBIENT_LIGHT: {
			GzLight* lightAmb = (GzLight*)valueList[index];
			ambientlight.direction[0] = lightAmb->direction[0];
			ambientlight.direction[1] = lightAmb->direction[1];
			ambientlight.direction[2] = lightAmb->direction[2];
			ambientlight.color[0] = lightAmb->color[0];
			ambientlight.color[1] = lightAmb->color[1];
			ambientlight.color[2] = lightAmb->color[2];
		}
		break;
		case GZ_AMBIENT_COEFFICIENT: {
			GzColor* colorAmb = (GzColor*)valueList[index];
			Ka[0] = (*colorAmb)[0];
			Ka[1] = (*colorAmb)[1];
			Ka[2] = (*colorAmb)[2];
		}
		break;
		case GZ_DIFFUSE_COEFFICIENT: {
			GzColor* colorDiff = (GzColor*)valueList[index];
			Kd[0] = (*colorDiff)[0];
			Kd[1] = (*colorDiff)[1];
			Kd[2] = (*colorDiff)[2];
		}
		break;
		case GZ_SPECULAR_COEFFICIENT: {
			GzColor* colorSpec = (GzColor*)valueList[index];
			Ks[0] = (*colorSpec)[0];
			Ks[1] = (*colorSpec)[1];
			Ks[2] = (*colorSpec)[2];
		}
		break;
		case GZ_DISTRIBUTION_COEFFICIENT: {
			float* power = (float*)valueList[index];
			spec = *power;
		}
		break;
		default:
		break;
		}
		index++;
	}
	
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	//if (numParts == 1) {
		GzCoord* verticesPointer = (GzCoord*)valueList[0];
		GzCoord* normalsPointer = (GzCoord*)valueList[1];
		GzCoord vertices[3], normals[3];

		// Construct 4D vector:
		float vertices4D[3][4], normals4D[3][4];
		float transedVertices4D[3][4], transedNormals4D[3][4];
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				vertices4D[j][i] = verticesPointer[j][i];
				normals4D[j][i] = normalsPointer[j][i];
			}
			vertices4D[j][3] = 1.0f;
			normals4D[j][3] = 1.0f;
		}
		// Transformation M * Vertex Coord.
		for (int k = 0; k < 3; k++) {
			for (int j = 0; j < 4; j++) {
				float sumV = 0, sumN = 0;
				for (int i = 0; i < 4; i++) {
					sumV += Ximage[matlevel][j][i] * vertices4D[k][i];
					sumN += Xnorm[matlevelNormal][j][i] * normals4D[k][i];
				}
				transedVertices4D[k][j] = sumV;
				transedNormals4D[k][j] = sumN;
			}
		}
		// 4D => 3D
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				vertices[j][i] = transedVertices4D[j][i] / transedVertices4D[j][3];
				normals[j][i] = transedNormals4D[j][i] / transedNormals4D[j][3];
			}
		}

		// Begin Rasterization:
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
		if ((int)(vertices[0][1] + 0.5) == (int)(vertices[1][1] + 0.5)) {
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
		else if ((int)(vertices[1][1] + 0.5) == (int)(vertices[2][1] + 0.5)) {
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
			float slopeOfSideEdge, middleX;
			if ((int)(vertices[0][0] + 0.5) == (int)(vertices[2][0] + 0.5)) {
				middleX = vertices[0][0];
			}
			else {
				slopeOfSideEdge = (vertices[0][1] - vertices[2][1]) / (vertices[0][0] - vertices[2][0]);
				middleX = (vertices[1][1] - vertices[0][1]) / slopeOfSideEdge + vertices[0][0];
			}

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
		float deltaX12, deltaY12, deltaX23, deltaY23, deltaX31, deltaY31;
		float A12, B12, C12, A23, B23, C23, A31, B31, C31;

		deltaX12 = vertices[1][0] - vertices[0][0];
		deltaY12 = vertices[1][1] - vertices[0][1];
		deltaX23 = vertices[2][0] - vertices[1][0];
		deltaY23 = vertices[2][1] - vertices[1][1];
		deltaX31 = vertices[0][0] - vertices[2][0];
		deltaY31 = vertices[0][1] - vertices[2][1];

		A12 = deltaY12;
		B12 = -1.0f * deltaX12;
		C12 = deltaX12 * vertices[0][1] - deltaY12 * vertices[0][0];
		A23 = deltaY23;
		B23 = -1.0f * deltaX23;
		C23 = deltaX23 * vertices[1][1] - deltaY23 * vertices[1][0];
		A31 = deltaY31;
		B31 = -1.0f * deltaX31;
		C31 = deltaX31 * vertices[2][1] - deltaY31 * vertices[2][0];
		// Get the current plane to interpolate Z:
		float X1 = vertices[1][0] - vertices[0][0];
		float Y1 = vertices[1][1] - vertices[0][1];
		float Z1 = vertices[1][2] - vertices[0][2];
		float X2 = vertices[2][0] - vertices[0][0];
		float Y2 = vertices[2][1] - vertices[0][1];
		float Z2 = vertices[2][2] - vertices[0][2];
		float planeA = (Y1 * Z2) - (Z1 * Y2);
		float planeB = -((X1 * Z2) - (Z1 * X2));
		float planeC = (X1 * Y2) - (Y1 * X2);
		float planeD = -1.0f * (planeA * vertices[0][0] + planeB * vertices[0][1] + planeC * vertices[0][2]);

		// Get Bounding Box:
		float minX = min(min(vertices[0][0], vertices[1][0]), vertices[2][0]);
		float maxX = max(max(vertices[0][0], vertices[1][0]), vertices[2][0]);
		float minY = min(min(vertices[0][1], vertices[1][1]), vertices[2][1]);
		float maxY = max(max(vertices[0][1], vertices[1][1]), vertices[2][1]);
		int minXPixel = (int)(minX + 0.5);
		int maxXPixel = (int)(maxX + 0.5);
		int minYPixel = (int)(minY + 0.5);
		int maxYPixel = (int)(maxY + 0.5);
		// Start Rasterization:
		for (int i = minXPixel; i <= maxXPixel; i++) {
			for (int j = minYPixel; j <= maxYPixel; j++) {
				//int currentIndex = ARRAY(i, j);
				float LEE12 = A12 * (float)i + B12 * (float)j + C12;
				float LEE23 = A23 * (float)i + B23 * (float)j + C23;
				float LEE31 = A31 * (float)i + B31 * (float)j + C31;

				if ((LEE12 > 0 && LEE23 > 0 && LEE31 > 0 && planeC != 0)
					|| (LEE12 < 0 && LEE23 < 0 && LEE31 < 0 && planeC != 0)
					|| LEE12 == 0 || LEE23 == 0 || LEE31 == 0) { // Any pixel inside or on the 3 edges.
					float interpolatedZ = -1.0f * (planeA * (float)i + planeB * (float)j + planeD) / planeC;
					int currentZ = (int)(interpolatedZ + 0.5);
					if (currentZ >= 0) {
						GzIntensity redIntensity, greenIntensity, blueIntensity;

						redIntensity = ctoi(flatcolor[0]);
						greenIntensity = ctoi(flatcolor[1]);
						blueIntensity = ctoi(flatcolor[2]);
						// Call GzPut to push the pixel to pixelbuffer.
						GzPut(i, j, redIntensity, greenIntensity, blueIntensity, 1, currentZ);
					}
				}
			}
		}
	//}
	return GZ_SUCCESS;
}

int pushMatToStack(short &head, GzMatrix* matStack, GzMatrix mat) {

	if (head < MATLEVELS) {
		if (head == -1) {
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < 4; i++) {
					matStack[0][i][j] = mat[i][j];
				}
			}
		}
		else {
			// Matrix Multiplication:
			for (int k = 0; k < 4; k++) {
				for (int j = 0; j < 4; j++) {
					float sum = 0;
					for (int i = 0; i < 4; i++) {
						sum += matStack[head][k][i] * mat[i][j];
					}
					matStack[head + 1][k][j] = sum;
				}
			}
		}
		// increment head
		head++;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int popMatToStack(short &head, GzMatrix* matStack) {
	if (head > -1) {
		head--;
	}
	else
		return GZ_FAILURE;

	return GZ_SUCCESS;
}