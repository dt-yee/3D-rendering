/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#define pi 3.1415926
short	ctoi(float color);
void NormalizeVec(GzCoord v);
float Size(GzCoord A, GzCoord B, GzCoord C);
void ComputeColor(GzRender *render, GzColor color, GzCoord coord);
float ScalarProduct(GzCoord vec1, GzCoord vec2);
void InterpolateCoefficients(GzCoord* coord, float* A, float* B, float* C, float* D);
GzCoord* GetaCoord(GzCoord* coord, float v1, float v2, float v3);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	//1  0   0    0
	//0  cos -sin 0
	//0  sin cos  0
	//0  0   0    1
	if (mat == NULL) return GZ_FAILURE;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	//float pi = 3.14159265358979;
	float radian = degree * (pi/180);

	mat[0][0] = 1.0;
	mat[1][1] = cos(radian);
	mat[1][2] = -sin(radian);
	mat[2][1] = sin(radian);
	mat[2][2] = cos(radian);
	mat[3][3] = 1.0;
	
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	if (mat == NULL) return GZ_FAILURE;
	//cos  0  sin 0
	//0    1  0   0
	//-sin 0  cos 0
	//0    0  0   1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	//double pi = 3.14159265358979;
	double radian = degree * (pi / 180);
	mat[0][0] = cos(radian);
	mat[0][2] = sin(radian);
	mat[1][1] = 1.0;
	mat[2][0] = -sin(radian);
	mat[2][2] = cos(radian);
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	if (mat == NULL) return GZ_FAILURE;
	//cos  -sin  0  0
	//sin   cos  0  0
	//0    0     1  0
	//0    0     0  1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	//double pi = 3.14159265358979;
	double radian = degree * (pi / 180);
	mat[0][0] = cos(radian);
	mat[0][1] = -sin(radian);
	mat[1][0] = sin(radian);
	mat[1][1] = cos(radian);
	mat[2][2] = 1.0;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	// 1 0 0 tx
	// 0 1 0 ty
	// 0 0 1 tz
	// 0 0 0 1
	if (mat == NULL) return GZ_FAILURE;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
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


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	if (mat == NULL) return GZ_FAILURE;
	//s1 0 0 0
	//0 s2 0 0
	//0 0 s3 0
	//0 0 0  1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0.0;
		}
	}
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1.0;
	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 
	*render = new GzRender();
	if (display == NULL) return GZ_FAILURE;

	(*render)->display = display;
	
	//init camera
	/* Camera defaults in rend.h*/
	(*render)->camera.FOV = DEFAULT_FOV;
	/* world coords for image plane origin */
	(*render)->camera.position[Z] = DEFAULT_IM_Z;
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
	(*render)->camera.position[X] = DEFAULT_IM_X;
	/* default look-at point = 0,0,0 */
	(*render)->camera.lookat[Z] = 0.0;
	(*render)->camera.lookat[Y] = 0.0;
	(*render)->camera.lookat[X] = 0.0;

	(*render)->camera.worldup[Z] = 0.0;
	(*render)->camera.worldup[Y] = 1.0;
	(*render)->camera.worldup[X] = 0.0;

	//init stack
	(*render)->matlevel = 0;
	//init xsp xpi xiw
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*render)->Xsp[i][j] = 0.0;
			(*render)->camera.Xpi[i][j] = 0.0;
			(*render)->camera.Xiw[i][j] = 0.0;
		}
	}

	//new parameter
	(*render)->interp_mode = GZ_RGB_COLOR;
	(*render)->numlights = 0;
	
	//default ambient
	(*render)->Ka[0] = 0.1;
	(*render)->Ka[1] = 0.1;
	(*render)->Ka[2] = 0.1;

	//default diffuse
	(*render)->Kd[0] = 0.7;
	(*render)->Kd[1] = 0.6;
	(*render)->Kd[2] = 0.5;

	// default specular
	(*render)->Ks[0] = 0.2;
	(*render)->Ks[1] = 0.3;
	(*render)->Ks[2] = 0.4;

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render == NULL) return GZ_FAILURE;

	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	if (render == NULL)	return GZ_FAILURE;
	GzInitDisplay(render->display);
	if (render->display == NULL) return GZ_FAILURE;

	//order: push Xsp, Xpi, Xiw

	//set Xsp
	//Xsp = xs / 2      0       0     xs / 2
	//		0		- ys / 2    0     ys / 2
	//		0			0    intmax     0
	//		0			0       0       1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			render->Xsp[i][j] = 0.0;
		}
	}
	render->Xsp[0][0] = render->display->xres / 2.0;
	render->Xsp[0][3] = render->display->xres / 2.0;
	render->Xsp[1][1] = -render->display->yres / 2.0;
	render->Xsp[1][3] = render->display->yres / 2.0;
	render->Xsp[2][2] = 2147483647;
	render->Xsp[3][3] = 1.0;
	//push Xsp
	GzPushMatrix(render, render->Xsp);


	//set Xpi
	//Xpi = 1       0      0     0
	//	    0       1      0     0
	//	    0       0    1 / d    0
	//	    0       0    1 / d    1
	//1/d=tan(FOV/2)
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			render->camera.Xpi[i][j] = 0.0;
		}
	}
	float FOV = render->camera.FOV * (pi / 180);
	float d = 1 / (tan(FOV / 2.0));
	render->camera.Xpi[0][0] = 1.0;
	render->camera.Xpi[1][1] = 1.0;
	render->camera.Xpi[2][2] = 1.0 / d;
	render->camera.Xpi[3][2] = 1.0 / d;
	render->camera.Xpi[3][3] = 1.0;
	//push Xpi
	GzPushMatrix(render, render->camera.Xpi);

	//set Xiw
	GzCoord cl,x_axis,y_axis,z_axis;
	cl[X] = render->camera.lookat[X] - render->camera.position[X];
	cl[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	cl[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	NormalizeVec(cl);
	//z axis
	//Z = CL / || CL ||
	z_axis[X] = cl[X];
	z_axis[Y] = cl[Y];
	z_axis[Z] = cl[Z];
	
	//y axis
	//Y = up' / || up'||
	//up�� = up - (up x Z) Z
	
	float up_z = render->camera.worldup[X] * z_axis[X] + render->camera.worldup[Y] * z_axis[Y] + render->camera.worldup[Z] * z_axis[Z];
	y_axis[X] = render->camera.worldup[X] - up_z*z_axis[X];
	y_axis[Y] = render->camera.worldup[Y] - up_z*z_axis[Y];
	y_axis[Z] = render->camera.worldup[Z] - up_z*z_axis[Z];
	NormalizeVec(y_axis);

	//x axis
	//X = (Y x Z)
	x_axis[X] = y_axis[Y] * z_axis[Z] - z_axis[Y] * y_axis[Z];
	x_axis[Y] = y_axis[Z] * z_axis[X] - z_axis[Z] * y_axis[X];
	x_axis[Z] = y_axis[X] * z_axis[Y] - z_axis[X] * y_axis[Y];
	//Xiw  =    Xx       Xy      Xz    -X.C
	//			Yx       Yy      Yz    -Y.C
	//			Zx       Zy      Zz    -Z.C
	//			0         0       0      1
	render->camera.Xiw[0][0] = x_axis[X];
	render->camera.Xiw[0][1] = x_axis[Y];
	render->camera.Xiw[0][2] = x_axis[Z];
	render->camera.Xiw[0][3] = -(x_axis[X] * render->camera.position[X] + x_axis[Y] * render->camera.position[Y] + x_axis[Z] * render->camera.position[Z]);
	render->camera.Xiw[1][0] = y_axis[X];
	render->camera.Xiw[1][1] = y_axis[Y];
	render->camera.Xiw[1][2] = y_axis[Z];
	render->camera.Xiw[1][3] = -(y_axis[X] * render->camera.position[X] + y_axis[Y] * render->camera.position[Y] + y_axis[Z] * render->camera.position[Z]);
	render->camera.Xiw[2][0] = z_axis[X];
	render->camera.Xiw[2][1] = z_axis[Y];
	render->camera.Xiw[2][2] = z_axis[Z];
	render->camera.Xiw[2][3] = -(z_axis[X] * render->camera.position[X] + z_axis[Y] * render->camera.position[Y] + z_axis[Z] * render->camera.position[Z]);
	render->camera.Xiw[3][0] = 0.0;
	render->camera.Xiw[3][1] = 0.0;
	render->camera.Xiw[3][2] = 0.0;
	render->camera.Xiw[3][3] = 1.0;
	//push Xiw
	GzPushMatrix(render, render->camera.Xiw);

	
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if (render == NULL || camera == NULL) return GZ_FAILURE;

	render->camera.FOV = camera->FOV;
	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];
	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];
	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	if (render == NULL || matrix == NULL) return GZ_FAILURE;
	
	if (render->matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}
	//overflow
	else if (render->matlevel == 0) {
		memcpy(render->Ximage[render->matlevel], matrix, sizeof(GzMatrix));
	}
	else {
		//debug
		//memcpy(render->Ximage[render->matlevel], matrix, sizeof(GzMatrix));
		
		//multiply before store
		//init
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				render->Ximage[render->matlevel][i][j] = 0.0;
			}
		}
		//multiply
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					render->Ximage[render->matlevel][i][j] += render->Ximage[render->matlevel-1][i][k] * matrix[k][j];
				}
			}
		}
	}

	//Xnorm
	if (render->matlevel == 0 || render->matlevel == 1) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) render->Xnorm[render->matlevel][i][j] = 1;
				else render->Xnorm[render->matlevel][i][j] = 0;
			}
		}
	}
	else {
		//in the matrix
		//	use any row / col and compute scale factor
		//	K = (a2 + b2 + c2)1 / 2
		//	divide all elements  of 3x3 R : R' = R/K
		//	R' is normalized (unitary) rotation matrix 
		//	Push R' onto Xn*
		float K = sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
		GzMatrix R;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				R[i][j] = matrix[i][j] / K;
			}
		}
		R[0][3] = 0;
		R[1][3] = 0;
		R[2][3] = 0;
		R[3][3] = 1;
		R[3][0] = 0;
		R[3][1] = 0;
		R[3][2] = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				render->Xnorm[render->matlevel][i][j] = 0.0;
			}
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					render->Xnorm[render->matlevel][i][j] += render->Xnorm[render->matlevel - 1][i][k] * R[k][j];
				}
			}
		}
	}

	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render == NULL) return GZ_FAILURE;
	//overflow
	if (render->matlevel <= 0) return GZ_FAILURE;
	render->matlevel -= 1;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	if (render == NULL || nameList == NULL || valueList == NULL) return GZ_FAILURE;
	//	GzColor* color = (GzColor*)valueList;

	//GzPutAttribute() must accept the following tokens / values:
	//GZ_RGB_COLOR	GzColor default flat - shade color
	//GZ_INTERPOLATE	int shader interpolation mode
	//GZ_DIRECTIONAL_LIGHT	GzLight
	//GZ_AMBIENT_LIGHT            	GzLight(ignore direction)
	//GZ_AMBIENT_COEFFICIENT	GzColor  Ka reflectance
	//GZ_DIFFUSE_COEFFICIENT	GzColor  Kd reflectance
	//GZ_SPECULAR_COEFFICIENT         GzColor Ks coef's
	//GZ_DISTRIBUTION_COEFFICIENT     float	spec power

	for (int i = 0; i < numAttributes; i++) {
		if (nameList[i] == GZ_RGB_COLOR) {
			GzColor* color = (GzColor*)valueList[i];
			render->flatcolor[0] = color[0][0];
			render->flatcolor[1] = color[0][1];
			render->flatcolor[2] = color[0][2];
		}
		else if (nameList[i] == GZ_INTERPOLATE) {
			render->interp_mode = *((int*)(valueList[i]));
		}
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT) {
			GzLight* light = (GzLight*)(valueList[i]);
			if (render->numlights < MAX_LIGHTS) {
				memcpy(render->lights[render->numlights].color, light->color, sizeof(GzLight));
				
				NormalizeVec(light->direction);
				memcpy(render->lights[render->numlights].direction, light->direction, sizeof(GzLight));
				render->numlights++;
			}
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT) {
			GzLight* light = (GzLight*)(valueList[i]);
			//memcpy(render->ambientlight.color, light, sizeof(GzLight));
			render->ambientlight = *light;
		}
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT) {
			GzColor* color = (GzColor*)(valueList[i]);
			/*memcpy(render->Ka, valueList[i], sizeof(GzLight));*/
			render->Ka[0] = color[0][0];
			render->Ka[1] = color[0][1];
			render->Ka[2] = color[0][2];
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT) {
			//GzColor* color = (GzColor*)(valueList[i]);
			//memcpy(render->Kd, valueList[i], sizeof(GzColor));
			GzColor* color = (GzColor*)valueList[i];
			render->Kd[0] = color[0][0];
			render->Kd[1] = color[0][1];
			render->Kd[2] = color[0][2];
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT) {
			GzColor* color = (GzColor*)(valueList[i]);
			//memcpy(render->Ks, valueList[i], sizeof(GzColor));
			render->Ks[0] = color[0][0];
			render->Ks[1] = color[0][1];
			render->Ks[2] = color[0][2];
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT) {
			render->spec = *((float*)(valueList[i]));
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	if (render == NULL || nameList == NULL || valueList == NULL) return GZ_FAILURE;
	GzCoord* coord = new GzCoord[3];
	GzCoord* norm = new GzCoord[3];
	//GzCoord* norm;
	for (int i = 0; i < numParts; i++) {
		if (nameList[i] == GZ_POSITION) {
			coord = (GzCoord*)valueList[i];
			GzMatrix topMatrix;
			memcpy(topMatrix, render->Ximage[render->matlevel - 1], sizeof(GzMatrix));
			float w = 0.0;
			GzCoord* tempcoord = new GzCoord[3];
			for (int i = 0; i < 3; i++) {
				tempcoord[i][X] = topMatrix[0][0] * coord[i][X] + topMatrix[0][1] * coord[i][Y] + topMatrix[0][2] * coord[i][Z] + topMatrix[0][3] * 1.0;
				tempcoord[i][Y] = topMatrix[1][0] * coord[i][X] + topMatrix[1][1] * coord[i][Y] + topMatrix[1][2] * coord[i][Z] + topMatrix[1][3] * 1.0;
				tempcoord[i][Z] = topMatrix[2][0] * coord[i][X] + topMatrix[2][1] * coord[i][Y] + topMatrix[2][2] * coord[i][Z] + topMatrix[2][3] * 1.0;
				w = topMatrix[3][0] * coord[i][X] + topMatrix[3][1] * coord[i][Y] + topMatrix[3][2] * coord[i][Z] + topMatrix[3][3] * 1.0;
				coord[i][X] = tempcoord[i][X] / w;
				coord[i][Y] = tempcoord[i][Y] / w;
				coord[i][Z] = tempcoord[i][Z] / w;
			}
			//skip any triangle with a negative screen-z vertex
			if ((coord[0][Z] < 0) || (coord[1][Z] < 0) || (coord[2][Z] < 0)) {
				continue;
			}
			//skip triangle outside the view
			if ((coord[0][X] < 0 || coord[0][X] > render->display->xres) || (coord[0][Y] < 0 || coord[0][Y] > render->display->yres)) {
				if ((coord[1][X] < 0 || coord[1][X] > render->display->xres) || (coord[1][Y] < 0 || coord[1][Y] > render->display->yres)) {
					if ((coord[2][X] < 0 || coord[2][X] > render->display->xres) || (coord[2][Y] < 0 || coord[2][Y] > render->display->yres)) {
						continue;
					}
				}
			}
		}
		if (nameList[i] == GZ_NORMAL) {
			GzMatrix topMatrix;
			GzCoord* tempnorm = (GzCoord*)valueList[i];
			NormalizeVec(tempnorm[0]);
			NormalizeVec(tempnorm[1]);
			NormalizeVec(tempnorm[2]);
			memcpy(topMatrix, render->Xnorm[render->matlevel - 1], sizeof(GzMatrix));

			//float w = 0.0;
			//
			//GzCoord* tempcoord = new GzCoord[3];
			for (int i = 0; i < 3; i++) {
				norm[i][X] = topMatrix[0][0] * tempnorm[i][X] + topMatrix[0][1] * tempnorm[i][Y] + topMatrix[0][2] * tempnorm[i][Z] + topMatrix[0][3] * 1.0;
				norm[i][Y] = topMatrix[1][0] * tempnorm[i][X] + topMatrix[1][1] * tempnorm[i][Y] + topMatrix[1][2] * tempnorm[i][Z] + topMatrix[1][3] * 1.0;
				norm[i][Z] = topMatrix[2][0] * tempnorm[i][X] + topMatrix[2][1] * tempnorm[i][Y] + topMatrix[2][2] * tempnorm[i][Z] + topMatrix[2][3] * 1.0;
				//w = topMatrix[3][0] * coord[i][X] + topMatrix[3][1] * coord[i][Y] + topMatrix[3][2] * coord[i][Z] + topMatrix[3][3] * 1.0;
				//norm[i][X] = tempcoord[i][X] / w;
				//norm[i][Y] = tempcoord[i][Y] / w;
				//norm[i][Z] = tempcoord[i][Z] / w;
			}
			NormalizeVec(norm[0]);
			NormalizeVec(norm[1]);
			NormalizeVec(norm[2]);
		}
	}
		//sort verts by Y
		int Ymin = 0, Ymax = 2;
		for (int j = 0; j < 3; j++) {
			if (coord[j][Y] < coord[Ymin][Y]) Ymin = j;
			if (coord[j][Y] > coord[Ymax][Y]) Ymax = j;
		}
		int Ymid = 3 - Ymin - Ymax;

		//set area
		int Bottom = floor(coord[Ymin][Y]);
		int Top = ceil(coord[Ymax][Y]);
		if (Bottom < 0) Bottom = 0;
		if (Top > render->display->yres) Top = render->display->yres;

		int Xmin = 0, Xmax = 0;
		for (int j = 0; j < 3; j++) {
			if (coord[j][X] < coord[Xmin][X]) Xmin = j;
			if (coord[j][X] > coord[Xmax][X]) Xmax = j;
		}
		int Left = floor(coord[Xmin][X]);
		int Right = ceil(coord[Xmax][X]);
		if (Left < 0) Left = 0;
		if (Right > render->display->xres) Right = render->display->xres;




		// I dont need find the clockwise 
		int X1 = 0, X2 = 1, X3 = 2;

		//interpolate Z
		//Ax + By + Cz + D = 0
		//The cross - product of two tri edges produces the(A, B, C) vector
		//Create edge vectors (X,Y,Z)0 and (X,Y,Z)1 from any two pairs of verts
		//(X, Y, Z)0 X(X, Y, Z)1 = (A, B, C) = norm to plane of edges(and tri)
		GzCoord E12, E23;
		E12[X] = coord[X2][X] - coord[X1][X];
		E12[Y] = coord[X2][Y] - coord[X1][Y];
		E12[Z] = coord[X2][Z] - coord[X1][Z];
		E23[X] = coord[X3][X] - coord[X2][X];
		E23[Y] = coord[X3][Y] - coord[X2][Y];
		E23[Z] = coord[X3][Z] - coord[X2][Z];
		float A = E12[Y] * E23[Z] - E12[Z] * E23[Y];
		float B = E12[Z] * E23[X] - E12[X] * E23[Z];
		float C = E12[X] * E23[Y] - E12[Y] * E23[X];
		float D = -(A*coord[X1][X]) - B*coord[X1][Y] - C*coord[X1][Z];
		GzColor c_v[3];

		//compute color of each point
		ComputeColor(render, c_v[0], norm[X1]);
		ComputeColor(render, c_v[1], norm[X2]);
		ComputeColor(render, c_v[2], norm[X3]);

		//substitue z with r/g/b/nx/ny/nz to get coefficients
		GzCoord *coordR = GetaCoord(coord, c_v[0][0], c_v[1][0], c_v[2][0]);
		GzCoord *coordG = GetaCoord(coord, c_v[0][1], c_v[1][1], c_v[2][1]);
		GzCoord *coordB = GetaCoord(coord, c_v[0][2], c_v[1][2], c_v[2][2]);
		GzCoord *coordNx = GetaCoord(coord, norm[X1][0], norm[X2][0], norm[X3][0]);
		GzCoord *coordNy = GetaCoord(coord, norm[X1][1], norm[X2][1], norm[X3][1]);
		GzCoord *coordNz = GetaCoord(coord, norm[X1][2], norm[X2][2], norm[X3][2]);
		
		
		float Ar = 0.0, Ag = 0.0, Ab = 0.0, Anx = 0.0, Any = 0.0, Anz = 0.0;
		float Br = 0.0, Bg = 0.0, Bb = 0.0, Bnx = 0.0, Bny = 0.0, Bnz = 0.0;
		float Cr = 0.0, Cg = 0.0, Cb = 0.0, Cnx = 0.0, Cny = 0.0, Cnz = 0.0;
		float Dr = 0.0, Dg = 0.0, Db = 0.0, Dnx = 0.0, Dny = 0.0, Dnz = 0.0;
		
		InterpolateCoefficients(coordR, &Ar, &Br, &Cr, &Dr);
		InterpolateCoefficients(coordG, &Ag, &Bg, &Cg, &Dg);
		InterpolateCoefficients(coordB, &Ab, &Bb, &Cb, &Db);
		InterpolateCoefficients(coordNx, &Anx, &Bnx, &Cnx, &Dnx);
		InterpolateCoefficients(coordNy, &Any, &Bny, &Cny, &Dny);
		InterpolateCoefficients(coordNz, &Anz, &Bnz, &Cnz, &Dnz);

		//float S = Size(coord[X1], coord[X2], coord[X3]);




		float Zbuffer = 0;
		for (int m = Left; m < Right; m++) {
			for (int n = Bottom; n < Top; n++) {
				float m1 = static_cast<float>(m);
				float n1 = static_cast<float>(n);

				//Compute LEE result for all three edges
				//E(x, y) = dY(x - X) - dX(y - Y)
				float s12 = (coord[X2][Y] - coord[X1][Y])*(m1 - coord[X1][X]) - (coord[X2][X] - coord[X1][X])*(n1 - coord[X1][Y]);
				float s23 = (coord[X3][Y] - coord[X2][Y])*(m1 - coord[X2][X]) - (coord[X3][X] - coord[X2][X])*(n1 - coord[X2][Y]);
				float s31 = (coord[X1][Y] - coord[X3][Y])*(m1 - coord[X3][X]) - (coord[X1][X] - coord[X3][X])*(n1 - coord[X3][Y]);

				//= 0 for points(x, y) on line(use E3 for this example)
				//= +for points in half - plane to right / below line
				//= -for points in half - plane to left / above line
				if (((s12 > 0) && (s23 > 0) && (s31 > 0)) || ((s12 < 0) && (s23 < 0) && (s31 < 0)) || s12 == 0 || s23 == 0 || s31 == 0) {
					Zbuffer = (-(A*m1) - (B*n1) - D) / C;
					//use GzGetDisplay in HW1 to get the pixel value to the display
					GzIntensity r, g, b, a;
					GzDepth z;
					GzCoord cur = { m,n,0 };
					GzGetDisplay(render->display, m, n, &r, &g, &b, &a, &z);

					if (Zbuffer < z) {
						if (render->interp_mode == GZ_COLOR) {
							//compute color of each point by plane equation
							float rf = (-(Ar*m1) - (Br*n1) - Dr) / Cr;
							float bf = (-(Ab*m1) - (Bb*n1) - Db) / Cb;
							float gf = (-(Ag*m1) - (Bg*n1) - Dg) / Cg;
							r = ctoi(rf);
							g = ctoi(gf);
							b = ctoi(bf);
							z = static_cast<int>(Zbuffer);
						}
						else if (render->interp_mode == GZ_NORMALS) {
							GzCoord Point_norm;
							GzColor Point_color;
							//compute point's normal by plane equation
							Point_norm[0] = (-(Anx*m1) - (Bnx*n1) - Dnx) / Cnx;
							Point_norm[1] = (-(Any*m1) - (Bny*n1) - Dny) / Cny;
							Point_norm[2] = (-(Anz*m1) - (Bnz*n1) - Dnz) / Cnz;
							NormalizeVec(Point_norm);

							ComputeColor(render, Point_color, Point_norm);
							r = ctoi(Point_color[0]);
							g = ctoi(Point_color[1]);
							b = ctoi(Point_color[2]);
							z = static_cast<int>(Zbuffer);
						}
						else {
							r = ctoi(render->flatcolor[0]);
							g = ctoi(render->flatcolor[1]);
							b = ctoi(render->flatcolor[2]);
							z = static_cast<int>(Zbuffer);
						}
						GzPutDisplay(render->display, m, n, r, g, b, a, z);
					}
				}
			}
		}

	
	
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}



void NormalizeVec(GzCoord v)
{
	float sum = (v[X] * v[X]) + (v[Y] * v[Y]) + (v[Z] * v[Z]);
	float len = sqrt(sum);
	v[X] = v[X] / len;
	v[Y] = v[Y] / len;
	v[Z] = v[Z] / len;
}

float Size(GzCoord A, GzCoord B, GzCoord C) {
	float res = A[0] * B[1] + A[1] * C[0] + B[0] * C[1] - A[1] * B[0] - A[0] * C[1] - B[1] * C[0];
	if (res < 0) res = -1.0 * res;
	return res;
}

void ComputeColor(GzRender *render, GzColor color, GzCoord coord) {
	GzCoord E = { 0,0,-1 };
	GzCoord Light_Direction = { 0,0,0 };
	GzColor Light_Color = { 0,0,0 };
	GzColor SpecLight = { 0,0,0 };
	GzColor DifLight = { 0,0,0 };
	GzCoord norm = { 0,0,0 };
	GzCoord R = { 0,0,0 };


	for (int i = 0; i < 3; i++) {
		norm[i] = coord[i];
	}
//	NormalizeVec(norm);

	for (int i = 0; i < render->numlights; i++) {
		memcpy(Light_Color, render->lights[i].color, sizeof(GzColor));
		memcpy(Light_Direction, render->lights[i].direction, sizeof(GzCoord));
		float Product_NL = ScalarProduct(Light_Direction, norm);
		float Product_NE = ScalarProduct(E, norm);
		if (Product_NE * Product_NL < 0) continue;
		if (Product_NL < 0) {
			for (int i = 0; i < 3; i++) {
				norm[i] *= -1.0;
			}
			Product_NL *= -1.0;
		}
		for (int i = 0; i < 3; i++) {
			DifLight[i] += Light_Color[i] * Product_NL;
			//R = 2(N*L)N - L
			R[i] = 2.0 * Product_NL * norm[i] - Light_Direction[i];
		}
		NormalizeVec(R);
		float Product_RE = ScalarProduct(R, E);
		// range 0-1
		if (Product_RE < 0) Product_RE = 0.0;
		if (Product_RE > 1) Product_RE = 1.0;
		float P_RE = pow(Product_RE, render->spec);
		for (int i = 0; i < 3; i++) {
			SpecLight[i] += Light_Color[i] * P_RE;
		}
	}
	for (int i = 0; i < 3; i++) {
		color[i] = render->Ks[i] * SpecLight[i] + render->Kd[i] * DifLight[i] + render->Ka[i] * render->ambientlight.color[i];
		if (color[i] < 0) color[i] = 0;
		if (color[i] > 1) color[i] = 1;
	}


}

float ScalarProduct(GzCoord vec1, GzCoord vec2) {
	float product = vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
	return product;
}

void InterpolateCoefficients(GzCoord* coord, float* A, float* B, float* C, float* D)
{
	GzCoord E12, E23;
	E12[X] = coord[1][X] - coord[0][X];
	E12[Y] = coord[1][Y] - coord[0][Y];
	E12[Z] = coord[1][Z] - coord[0][Z];
	E23[X] = coord[2][X] - coord[1][X];
	E23[Y] = coord[2][Y] - coord[1][Y];
	E23[Z] = coord[2][Z] - coord[1][Z];
	*A = E12[Y] * E23[Z] - E12[Z] * E23[Y];
	*B = E12[Z] * E23[X] - E12[X] * E23[Z];
	*C = E12[X] * E23[Y] - E12[Y] * E23[X];
	*D = -((*A)*coord[0][X]) - (*B)*coord[0][Y] - (*C)*coord[0][Z];
}

GzCoord* GetaCoord(GzCoord* coord, float v1, float v2, float v3) {
	GzCoord* res = new GzCoord[3];
	for (int i = 0; i < 3; i++) {
		res[i][0] = coord[i][0];
		res[i][1] = coord[i][1];
	}
	res[0][2] = v1;
	res[1][2] = v2;
	res[2][2] = v3;
	return res;
}