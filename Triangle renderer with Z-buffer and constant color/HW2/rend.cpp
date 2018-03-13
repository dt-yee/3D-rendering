#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
short	ctoi(float color);

int GzNewRender(GzRender **render, GzDisplay *display)
{
/* 
- malloc a renderer struct
- span interpolator needs pointer to display for pixel writes
*/
	if (display == NULL) return GZ_FAILURE;

	*render = new GzRender();
	//check if lack of memory or other questions
	if (render == NULL) return GZ_FAILURE;
	//point to display
	(*render)->display = display;
	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render == NULL) return GZ_FAILURE;

	//avoid wild pointer
	render->display = NULL;
	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
	if (render == NULL) return GZ_FAILURE;
	GzInitDisplay(render->display);
	//check if initdisplay fail
	if (render->display == NULL) return GZ_FAILURE;
	return GZ_SUCCESS;
}

/*
*nameList: List of Tokens	(ints) GZ_RGB_COLOR
*valueList: List of values Float Color[3]
Only use GZ_RGB_COLOR for HW2
*/
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{

/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	if (render == NULL || nameList == NULL || valueList == NULL) return GZ_FAILURE;
//	GzColor* color = (GzColor*)valueList;
	for (int i = 0; i < numAttributes; i++) {
		if (nameList[i] == GZ_RGB_COLOR) {
			GzColor* color = (GzColor*)valueList[i];
			render->flatcolor[0] = color[i][0];
			render->flatcolor[1] = color[i][1];
			render->flatcolor[2] = color[i][2];
		}
	}
	return GZ_SUCCESS;
}

/*
only use GZ_POSITION token for HW2 
that token means valueList is a pointer to 3 vertex positions  XYZ, XYZ, XYZ
*/
int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions 
- Invoke the scan converter and return an error code
*/
	if (render == NULL || nameList == NULL || valueList == NULL) return GZ_FAILURE;

	for (int i = 0; i < numParts; i++) {
		if (nameList[i] == GZ_POSITION) {
			//float* coord = (float*)valueList[i];
			GzCoord* coord = (GzCoord*)valueList[i];
			//sort verts by Y
			int Ymin = 0, Ymax = 2;
			for (int j = 0 ; j < 3; j++) {
				if (coord[j][Y] < coord[Ymin][Y]) Ymin = j;
				if (coord[j][Y] > coord[Ymax][Y]) Ymax = j;
			}
			int Ymid = 3 - Ymin - Ymax;

			//set area
			int Bottom = floor(coord[Ymin][Y]);
			int Top = ceil(coord[Ymax][Y]);
			if (Bottom < 0) Bottom = 0;
			if (Top > render->display->yres) Top = render->display->yres;

			int Xmin = 0,  Xmax = 0;
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
					if (((s12 > 0) && (s23 > 0) && (s31 > 0)) || ((s12 < 0) && (s23 < 0) && (s31 < 0))) {
						Zbuffer = (-(A*m1) - (B*n1) - D) / C;
						//use GzGetDisplay in HW1 to get the pixel value to the display
						GzIntensity r, g, b, a;
						GzDepth z;
						GzGetDisplay(render->display, m, n, &r, &g, &b, &a, &z);
						//float tmp = static_cast<float>(z);
						if (Zbuffer < z) {
							r = ctoi(render->flatcolor[0]);
							g = ctoi(render->flatcolor[1]);
							b = ctoi(render->flatcolor[2]);
							z = static_cast<int>(Zbuffer);
							GzPutDisplay(render->display, m, n, r, g, b, a, z);
						}
					}
				}
			}

		}
	}
	return GZ_SUCCESS;
}



// should declear this function in the head of the cpp file 
// we can use this function instead of multiply 4095

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

