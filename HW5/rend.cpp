/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include	<algorithm>

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
	numlights = 0;
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
	GzMatrix matrix_I;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			matrix_I[j][i] = 0;
		}
	}
	matrix_I[0][0] = 1.0f;
	matrix_I[1][1] = 1.0f;
	matrix_I[2][2] = 1.0f;
	matrix_I[3][3] = 1.0f;

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
	int status = 0;
	status |= pushMatToStack(matlevel, Ximage, matrix);

	GzMatrix matrix_I;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			matrix_I[j][i] = 0;
		}
	}
	matrix_I[0][0] = 1.0f;
	matrix_I[1][1] = 1.0f;
	matrix_I[2][2] = 1.0f;
	matrix_I[3][3] = 1.0f;

	// For Xnorm: Normal Stack:
	if (matlevelNormal < 2) {	// Xsp and Xpi, Push I
		status |= pushMatToStack(matlevelNormal, Xnorm, matrix_I);
	}
	else if (matlevelNormal == 2) {	// Xiw.
		GzMatrix Xnorm_iw;
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				Xnorm_iw[j][i] = m_camera.Xiw[j][i];
			}
		}
		Xnorm_iw[0][3] = 0;
		Xnorm_iw[1][3] = 0;
		Xnorm_iw[2][3] = 0;

		status |= pushMatToStack(matlevelNormal, Xnorm, Xnorm_iw);
	}
	else {	// Only Allow Rotation Matrices.
		if (matrix[0][1] == 0 && matrix[0][2] == 0 && matrix[1][0] == 0
			&& matrix[1][2] == 0 && matrix[2][0] == 0 && matrix[2][1] == 0) {
			status |= pushMatToStack(matlevelNormal, Xnorm, matrix_I);
		}
		else {
			status |= pushMatToStack(matlevelNormal, Xnorm, matrix);
		}
	}

	if (status)
		return GZ_FAILURE;
	return GZ_SUCCESS;
	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (popMatToStack(matlevel, Ximage))
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
- GzPutAttribute() must accept the following tokens/values:

- GZ_RGB_COLOR					GzColor		default flat-shade color
- GZ_INTERPOLATE				int			shader interpolation mode
- GZ_DIRECTIONAL_LIGHT			GzLight
- GZ_AMBIENT_LIGHT            	GzLight		(ignore direction)
- GZ_AMBIENT_COEFFICIENT		GzColor		Ka reflectance
- GZ_DIFFUSE_COEFFICIENT		GzColor		Kd reflectance
- GZ_SPECULAR_COEFFICIENT       GzColor		Ks coef's
- GZ_DISTRIBUTION_COEFFICIENT   float		spec power
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
		case GZ_TEXTURE_MAP: {
			GzTexture textureFunc = (GzTexture)valueList[index];
			if (textureFunc != NULL)
				tex_fun = textureFunc;
		}
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
-- Return error code
*/
/*
-- Xform positions of verts using matrix on top of stack 
-- Clip - just discard any triangle with any vert(s) behind view plane 
		- optional: test for triangles with all three verts off-screen (trivial frustum cull)
-- invoke triangle rasterizer  
*/

	GzCoord* verticesPointer = (GzCoord*)valueList[0];
	GzCoord* normalsPointer = (GzCoord*)valueList[1];
	GzTextureIndex* uvListPointer = (GzTextureIndex*)valueList[2];
	GzCoord vertices[3], normals[3];
	GzTextureIndex uvList[3];
	
	/*
	char buffer[50];
	sprintf(buffer, "uv = %4.4f, %4.4f", uvListPointer[0][0], uvListPointer[0][1]);
	OutputDebugStringA(buffer);
	*/

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

	// Assign UV List:
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 2; i++) {
			uvList[j][i] = uvListPointer[j][i];
		}
	}

	// Begin Rasterization:
	if (vertices[0][1] > vertices[1][1]) {
		for (int i = 0; i < 3; i++) {
			std::swap(vertices[0][i], vertices[1][i]);
			std::swap(normals[0][i], normals[1][i]);
			if (i < 2)
				std::swap(uvList[0][i], uvList[1][i]);
		}
	}
	if (vertices[0][1] > vertices[2][1]) {
		for (int i = 0; i < 3; i++) {
			std::swap(vertices[0][i], vertices[2][i]);
			std::swap(normals[0][i], normals[2][i]);
			if (i < 2)
				std::swap(uvList[0][i], uvList[2][i]);
		}
	}
	if (vertices[1][1] > vertices[2][1]) {
		for (int i = 0; i < 3; i++) {
			std::swap(vertices[1][i], vertices[2][i]);
			std::swap(normals[1][i], normals[2][i]);
			if (i < 2)
				std::swap(uvList[1][i], uvList[2][i]);
		}
	}

	//sorted by Y. determine final order by middle-Y & special cases.
	if ((int)(vertices[0][1] + 0.5) == (int)(vertices[1][1] + 0.5)) {
		if (vertices[0][0] > vertices[1][0]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[1][i], vertices[2][i]);
				std::swap(normals[1][i], normals[2][i]);
				if (i < 2)
					std::swap(uvList[1][i], uvList[2][i]);
			}
		}
	}
	else if ((int)(vertices[1][1] + 0.5) == (int)(vertices[2][1] + 0.5)) {
		if (vertices[2][0] > vertices[1][0]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[1][i], vertices[2][i]);
				std::swap(normals[1][i], normals[2][i]);
				if (i < 2)
					std::swap(uvList[1][i], uvList[2][i]);
			}
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
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[1][i], vertices[2][i]);
				std::swap(normals[1][i], normals[2][i]);
				if (i < 2)
					std::swap(uvList[1][i], uvList[2][i]);
			}
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

	// Lighting of 3 Vertices:
	GzColor specIntensity[3], diffIntensity[3], ambIntensity[3], finalIntensity[3];
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) {
			specIntensity[j][i] = 0;
			diffIntensity[j][i] = 0;
			ambIntensity[j][i] = 0;
			finalIntensity[j][i] = 0;
		}
	}

	// For texture color of Gouraud Shading:
	/*
	GzColor uvVertexColor[3] = {0};
	if (interp_mode == GZ_COLOR && tex_fun != NULL) {
		//GzTextureIndex currentUV;
		
		//currentUV[U] = -1.0f * (uPlaneA * (float)i + uPlaneB * (float)j + uPlaneD) / uPlaneC;
		//currentUV[V] = -1.0f * (vPlaneA * (float)i + vPlaneB * (float)j + vPlaneD) / vPlaneC;

		int status = tex_fun(uvList[0][U], uvList[0][V], uvVertexColor[0]);
		status |= tex_fun(uvList[0][U], uvList[0][V], uvVertexColor[1]);
		status |= tex_fun(uvList[0][U], uvList[0][V], uvVertexColor[2]);
		if (status) {
			return GZ_FAILURE;
		}

	}
	*/
	if (interp_mode == GZ_COLOR) {	// Gouraud Shading without texture
		for (int j = 0; j < 3; j++) {	// j is vertex
			for (int i = 0; i < numlights; i++) {	// i is light
				GzCoord R, E;

				E[0] = 0;
				E[1] = 0;
				E[2] = -1.0f;

				float nDotL = normals[j][0] * (lights[i]).direction[0]
					+ normals[j][1] * (lights[i]).direction[1]
					+ normals[j][2] * (lights[i]).direction[2];
				float nDotE = normals[j][0] * E[0] + normals[j][1] * E[1] + normals[j][2] * E[2];

				if (nDotL * nDotE > 0) {
					for (int k = 0; k < 3; k++) {
						R[k] = 2.0f * nDotL * normals[j][k] - (lights[i]).direction[k];
					}
					float lengthR = (float)sqrt((double)(R[0] * R[0] + R[1] * R[1] + R[2] * R[2]));
					for (int k = 0; k < 3; k++) {
						R[k] /= lengthR;
					}

					for (int k = 0; k < 3; k++) {	// k is color - R G B
						float rDotE = R[0] * E[0] + R[1] * E[1] + R[2] * E[2];
						if (rDotE < 0) {
							rDotE = 0;
						}
						if (rDotE > 1.0f) {
							rDotE = 1.0f;
						}
						
						if (tex_fun != NULL) {
							Ks[k] = 1.0f;
							Kd[k] = 1.0f;
							Ka[k] = 1.0f;
						}
						

						specIntensity[j][k] += Ks[k] *
							(float)pow((double)(rDotE), (double)spec)
							* (lights[i]).color[k];
						if (nDotL > 0 && nDotE > 0) {
							diffIntensity[j][k] += Kd[k] *
								(normals[j][0] * (lights[i]).direction[0]
									+ normals[j][1] * (lights[i]).direction[1]
									+ normals[j][2] * (lights[i]).direction[2]) * (lights[i]).color[k];
						}
						else {
							diffIntensity[j][k] += Kd[k] *
								(-1.0f * normals[j][0] * (lights[i]).direction[0]
									- 1.0f * normals[j][1] * (lights[i]).direction[1]
									- 1.0f * normals[j][2] * (lights[i]).direction[2]) * (lights[i]).color[k];
						}
					}
				}
			}

			for (int k = 0; k < 3; k++) {
				ambIntensity[j][k] += Ka[k] * ambientlight.color[k];
			}

		}
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 3; i++) {
				finalIntensity[j][i] = specIntensity[j][i] + diffIntensity[j][i] + ambIntensity[j][i];
				if (finalIntensity[j][i] > 1.0) {
					finalIntensity[j][i] = 1.0;
				}
				if (finalIntensity[j][i] < 0) {
					finalIntensity[j][i] = 0;
				}
			}
		}
	}

	// Interpolate Red:
	float redX1 = vertices[1][0] - vertices[0][0];
	float redY1 = vertices[1][1] - vertices[0][1];
	float redZ1 = finalIntensity[1][0] - finalIntensity[0][0];
	float redX2 = vertices[2][0] - vertices[0][0];
	float redY2 = vertices[2][1] - vertices[0][1];
	float redZ2 = finalIntensity[2][0] - finalIntensity[0][0];
	float redPlaneA = (redY1 * redZ2) - (redZ1 * redY2);
	float redPlaneB = -((redX1 * redZ2) - (redZ1 * redX2));
	float redPlaneC = (redX1 * redY2) - (redY1 * redX2);
	float redPlaneD = -1.0f * (redPlaneA * vertices[0][0] + redPlaneB * vertices[0][1] + redPlaneC * finalIntensity[0][0]);

	// Interpolate Green:
	float greenX1 = vertices[1][0] - vertices[0][0];
	float greenY1 = vertices[1][1] - vertices[0][1];
	float greenZ1 = finalIntensity[1][1] - finalIntensity[0][1];
	float greenX2 = vertices[2][0] - vertices[0][0];
	float greenY2 = vertices[2][1] - vertices[0][1];
	float greenZ2 = finalIntensity[2][1] - finalIntensity[0][1];
	float greenPlaneA = (greenY1 * greenZ2) - (greenZ1 * greenY2);
	float greenPlaneB = -((greenX1 * greenZ2) - (greenZ1 * greenX2));
	float greenPlaneC = (greenX1 * greenY2) - (greenY1 * greenX2);
	float greenPlaneD = -1.0f * (greenPlaneA * vertices[0][0] + greenPlaneB * vertices[0][1] + greenPlaneC * finalIntensity[0][1]);

	// Interpolate Blue:
	float blueX1 = vertices[1][0] - vertices[0][0];
	float blueY1 = vertices[1][1] - vertices[0][1];
	float blueZ1 = finalIntensity[1][2] - finalIntensity[0][2];
	float blueX2 = vertices[2][0] - vertices[0][0];
	float blueY2 = vertices[2][1] - vertices[0][1];
	float blueZ2 = finalIntensity[2][2] - finalIntensity[0][2];
	float bluePlaneA = (blueY1 * blueZ2) - (blueZ1 * blueY2);
	float bluePlaneB = -((blueX1 * blueZ2) - (blueZ1 * blueX2));
	float bluePlaneC = (blueX1 * blueY2) - (blueY1 * blueX2);
	float bluePlaneD = -1.0f * (bluePlaneA * vertices[0][0] + bluePlaneB * vertices[0][1] + bluePlaneC * finalIntensity[0][2]);

	// Interpolate normalX:
	float normalX_X1 = vertices[1][0] - vertices[0][0];
	float normalX_Y1 = vertices[1][1] - vertices[0][1];
	float normalX_Z1 = normals[1][0] - normals[0][0];
	float normalX_X2 = vertices[2][0] - vertices[0][0];
	float normalX_Y2 = vertices[2][1] - vertices[0][1];
	float normalX_Z2 = normals[2][0] - normals[0][0];
	float normalX_PlaneA = (normalX_Y1 * normalX_Z2) - (normalX_Z1 * normalX_Y2);
	float normalX_PlaneB = -((normalX_X1 * normalX_Z2) - (normalX_Z1 * normalX_X2));
	float normalX_PlaneC = (normalX_X1 * normalX_Y2) - (normalX_Y1 * normalX_X2);
	float normalX_PlaneD = -1.0f * (normalX_PlaneA * vertices[0][0] + normalX_PlaneB * vertices[0][1] + normalX_PlaneC * normals[0][0]);

	// Interpolate normalY:
	float normalY_X1 = vertices[1][0] - vertices[0][0];
	float normalY_Y1 = vertices[1][1] - vertices[0][1];
	float normalY_Z1 = normals[1][1] - normals[0][1];
	float normalY_X2 = vertices[2][0] - vertices[0][0];
	float normalY_Y2 = vertices[2][1] - vertices[0][1];
	float normalY_Z2 = normals[2][1] - normals[0][1];
	float normalY_PlaneA = (normalY_Y1 * normalY_Z2) - (normalY_Z1 * normalY_Y2);
	float normalY_PlaneB = -((normalY_X1 * normalY_Z2) - (normalY_Z1 * normalY_X2));
	float normalY_PlaneC = (normalY_X1 * normalY_Y2) - (normalY_Y1 * normalY_X2);
	float normalY_PlaneD = -1.0f * (normalY_PlaneA * vertices[0][0] + normalY_PlaneB * vertices[0][1] + normalY_PlaneC * normals[0][1]);

	// Interpolate normalZ:
	float normalZ_X1 = vertices[1][0] - vertices[0][0];
	float normalZ_Y1 = vertices[1][1] - vertices[0][1];
	float normalZ_Z1 = normals[1][2] - normals[0][2];
	float normalZ_X2 = vertices[2][0] - vertices[0][0];
	float normalZ_Y2 = vertices[2][1] - vertices[0][1];
	float normalZ_Z2 = normals[2][2] - normals[0][2];
	float normalZ_PlaneA = (normalZ_Y1 * normalZ_Z2) - (normalZ_Z1 * normalZ_Y2);
	float normalZ_PlaneB = -((normalZ_X1 * normalZ_Z2) - (normalZ_Z1 * normalZ_X2));
	float normalZ_PlaneC = (normalZ_X1 * normalZ_Y2) - (normalZ_Y1 * normalZ_X2);
	float normalZ_PlaneD = -1.0f * (normalZ_PlaneA * vertices[0][0] + normalZ_PlaneB * vertices[0][1] + normalZ_PlaneC * normals[0][2]);
	/*
	GzColor vertTextColor[3] = {0};
	if (tex_fun != NULL) {

		int status = tex_fun(uvList[0][U], uvList[0][V], vertTextColor[0]);
		status |= tex_fun(uvList[1][U], uvList[1][V], vertTextColor[1]);
		status |= tex_fun(uvList[2][U], uvList[2][V], vertTextColor[2]);
		if (status) {
			return GZ_FAILURE;
		}

		//char buffer[50];
		//sprintf(buffer, "tex_fun = %4.4f, %4.4f", vertTextColor[0][0], vertTextColor[0][1]);
		//OutputDebugStringA(buffer);
	}
	*/

	// Perspective Correction:
	float vZPrime;
	GzTextureIndex perspUVList[3];
	for (int j = 0; j < 3; j++) {
		vZPrime = vertices[j][2] / ((float)MAXINT - vertices[j][2]);
		perspUVList[j][U] = uvList[j][U] / (vZPrime + 1.0f);
		perspUVList[j][V] = uvList[j][V] / (vZPrime + 1.0f);
	}

	// Interpolate U, V:
	float uX1 = vertices[1][0] - vertices[0][0];
	float uY1 = vertices[1][1] - vertices[0][1];
	float uZ1 = perspUVList[1][0] - perspUVList[0][0];
	float uX2 = vertices[2][0] - vertices[0][0];
	float uY2 = vertices[2][1] - vertices[0][1];
	float uZ2 = perspUVList[2][0] - perspUVList[0][0];
	float uPlaneA = (uY1 * uZ2) - (uZ1 * uY2);
	float uPlaneB = -((uX1 * uZ2) - (uZ1 * uX2));
	float uPlaneC = (uX1 * uY2) - (uY1 * uX2);
	float uPlaneD = -1.0f * (uPlaneA * vertices[0][0] + uPlaneB * vertices[0][1] + uPlaneC * perspUVList[0][0]);

	float vX1 = vertices[1][0] - vertices[0][0];
	float vY1 = vertices[1][1] - vertices[0][1];
	float vZ1 = perspUVList[1][1] - perspUVList[0][1];
	float vX2 = vertices[2][0] - vertices[0][0];
	float vY2 = vertices[2][1] - vertices[0][1];
	float vZ2 = perspUVList[2][1] - perspUVList[0][1];
	float vPlaneA = (vY1 * vZ2) - (vZ1 * vY2);
	float vPlaneB = -((vX1 * vZ2) - (vZ1 * vX2));
	float vPlaneC = (vX1 * vY2) - (vY1 * vX2);
	float vPlaneD = -1.0f * (vPlaneA * vertices[0][0] + vPlaneB * vertices[0][1] + vPlaneC * perspUVList[0][1]);
	

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

			if (((LEE12 > 0 && LEE23 > 0 && LEE31 > 0 && planeC != 0)
				|| (LEE12 < 0 && LEE23 < 0 && LEE31 < 0 && planeC != 0)
				|| (LEE12 == 0 || LEE23 == 0 || LEE31 == 0))
				&& redPlaneC * greenPlaneC * bluePlaneC != 0
				&& normalX_PlaneC * normalY_PlaneC * normalZ_PlaneC != 0) { // Any pixel inside or on the 3 edges.
				float interpolatedZ = -1.0f * (planeA * (float)i + planeB * (float)j + planeD) / planeC;
				int currentZ = (int)(interpolatedZ + 0.5);
				if (currentZ >= 0) {
					// Color Calculation:
					GzIntensity redIntensity = 0, greenIntensity = 0, blueIntensity = 0;

						if (interp_mode == GZ_FLAT) {	// Flat Shading
							redIntensity = ctoi(flatcolor[0]);
							greenIntensity = ctoi(flatcolor[1]);
							blueIntensity = ctoi(flatcolor[2]);
						}
						else if (interp_mode == GZ_COLOR) {	//Gauroud Shading
							GzColor intensity;
							intensity[0] = -1.0f * (redPlaneA * (float)i + redPlaneB * (float)j + redPlaneD) / redPlaneC;
							intensity[1] = -1.0f * (greenPlaneA * (float)i + greenPlaneB * (float)j + greenPlaneD) / greenPlaneC;
							intensity[2] = -1.0f * (bluePlaneA * (float)i + bluePlaneB * (float)j + bluePlaneD) / bluePlaneC;

							if (tex_fun != NULL) {
								GzTextureIndex currentUV;
								GzColor uvColor;
								float vzPrimeInterp = (float)currentZ / ((float)MAXINT - (float)currentZ);
								currentUV[U] = -1.0f * (uPlaneA * (float)i + uPlaneB * (float)j + uPlaneD) / uPlaneC;
								currentUV[V] = -1.0f * (vPlaneA * (float)i + vPlaneB * (float)j + vPlaneD) / vPlaneC;

								currentUV[U] = currentUV[U] * (vzPrimeInterp + 1.0f);
								currentUV[V] = currentUV[V] * (vzPrimeInterp + 1.0f);

								int status = tex_fun(currentUV[U], currentUV[V], uvColor);
								if (status) {
									return GZ_FAILURE;
								}

								for (int m = 0; m < 3; m++) {
									intensity[m] *= uvColor[m];
								}
							}

							redIntensity = ctoi(intensity[0]);
							greenIntensity = ctoi(intensity[1]);
							blueIntensity = ctoi(intensity[2]);

						}
						else if (interp_mode == GZ_NORMALS) {	// Phong Shading
							GzColor intensity;
							GzCoord interpolatedNormal;
							interpolatedNormal[0] = -1.0f * (normalX_PlaneA * (float)i + normalX_PlaneB * (float)j + normalX_PlaneD) / normalX_PlaneC;
							interpolatedNormal[1] = -1.0f * (normalY_PlaneA * (float)i + normalY_PlaneB * (float)j + normalY_PlaneD) / normalY_PlaneC;
							interpolatedNormal[2] = -1.0f * (normalZ_PlaneA * (float)i + normalZ_PlaneB * (float)j + normalZ_PlaneD) / normalZ_PlaneC;

							if (tex_fun != NULL) {
								GzTextureIndex currentUV;
								GzColor uvColor;
								float vzPrimeInterp = (float)currentZ / ((float)MAXINT - (float)currentZ);
								currentUV[U] = -1.0f * (uPlaneA * (float)i + uPlaneB * (float)j + uPlaneD) / uPlaneC;
								currentUV[V] = -1.0f * (vPlaneA * (float)i + vPlaneB * (float)j + vPlaneD) / vPlaneC;

								currentUV[U] = currentUV[U] * (vzPrimeInterp + 1.0f);
								currentUV[V] = currentUV[V] * (vzPrimeInterp + 1.0f);

								int status = tex_fun(currentUV[U], currentUV[V], uvColor);
								if (status) {
									return GZ_FAILURE;
								}
								
								for (int m = 0; m < 3; m++) {
									Kd[m] = uvColor[m];
									Ka[m] = uvColor[m];
								}
								
							}
							
							GzColor specI, diffI, ambI;
							for (int m = 0; m < 3; m++) {
								specI[m] = 0;
								diffI[m] = 0;
								ambI[m] = 0;
							}

							for (int m = 0; m < numlights; m++) {
								GzCoord R, E;

								E[0] = 0;
								E[1] = 0;
								E[2] = -1.0f;

								float nDotL = interpolatedNormal[0] * (lights[m]).direction[0]
									+ interpolatedNormal[1] * (lights[m]).direction[1]
									+ interpolatedNormal[2] * (lights[m]).direction[2];
								float nDotE = interpolatedNormal[0] * E[0] + interpolatedNormal[1] * E[1] + interpolatedNormal[2] * E[2];

								if (nDotL * nDotE > 0) {
									for (int k = 0; k < 3; k++) {
										R[k] = 2.0f * nDotL * interpolatedNormal[k] - (lights[m]).direction[k];
									}
									float lengthR = (float)sqrt((double)(R[0] * R[0] + R[1] * R[1] + R[2] * R[2]));
									for (int k = 0; k < 3; k++) {
										R[k] /= lengthR;
									}

									for (int k = 0; k < 3; k++) {
										float rDotE = R[0] * E[0] + R[1] * E[1] + R[2] * E[2];
										if (rDotE < 0) {
											rDotE = 0;
										}
										if (rDotE > 1.0f) {
											rDotE = 1.0f;
										}
										
										specI[k] += Ks[k] *
											(float)pow((double)(rDotE), (double)spec)
											* (lights[m]).color[k];
										
										if (nDotL > 0 && nDotE > 0) {
											diffI[k] += Kd[k] *
												(interpolatedNormal[0] * (lights[m]).direction[0]
													+ interpolatedNormal[1] * (lights[m]).direction[1]
													+ interpolatedNormal[2] * (lights[m]).direction[2]) * (lights[m]).color[k];
										}
										else {
											diffI[k] += Kd[k] *
												(-1.0f * interpolatedNormal[0] * (lights[m]).direction[0]
													- 1.0f * interpolatedNormal[1] * (lights[m]).direction[1]
													- 1.0f * interpolatedNormal[2] * (lights[m]).direction[2]) * (lights[m]).color[k];
										}
									}
								}
							}

							for (int m = 0; m < 3; m++) {
								ambI[m] += Ka[m] * ambientlight.color[m];
							}


							for (int m = 0; m < 3; m++) {
								intensity[m] = specI[m] + diffI[m] + ambI[m];
								if (intensity[m] > 1.0) {
									intensity[m] = 1.0;
								}
								if (intensity[m] < 0) {
									intensity[m] = 0;
								}
							}
							
							redIntensity = ctoi(intensity[0]);
							greenIntensity = ctoi(intensity[1]);
							blueIntensity = ctoi(intensity[2]);
							
						}
					

					// Call GzPut to push the pixel to pixelbuffer.
					GzPut(i, j, redIntensity, greenIntensity, blueIntensity, 1, currentZ);
				}
			}
		}
	}

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

