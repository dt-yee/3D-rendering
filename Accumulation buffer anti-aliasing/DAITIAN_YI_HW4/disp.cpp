/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	//only input width height be in valid range a framebuffer can be created.
	//pitfalls, check pixels-coords
	if ((width <= 0)|| (height <= 0) || (width > MAXXRES) || (height > MAXYRES)) {
		return GZ_FAILURE;
	}
	// we need one more space for '\0'!
	*framebuffer = new char[3 * width*height +1];

	//check if lack of memory or other questions
	if (framebuffer == NULL) return GZ_FAILURE;
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	//pitfalls. bounds check
	if ((xRes <= 0) || (yRes <= 0) || (xRes > MAXXRES) || (yRes > MAXYRES)) {
		return GZ_FAILURE;
	}

	*display = new GzDisplay();

	//check if lack of memory or other questions
	if (display == NULL) return GZ_FAILURE;

	//initialize
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = new GzPixel[xRes*yRes];

	//check if lack of memory or other questions
	if ((*display)->fbuf == NULL) return GZ_FAILURE;

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	if (display != NULL) {
		//usc delete[] to delete array space
		delete[] display->fbuf;
		// avoid wild pointer
		display->fbuf = NULL;
		delete display;
	}
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	if (display == NULL) return GZ_FAILURE;
	
	//get value
	*xRes = display->xres;
	*yRes = display->yres;

	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	if (display == NULL) return GZ_FAILURE;
	int size = static_cast<int>(display->xres * display->yres);
	for (int i = 0; i < size; i++) {
		//white background
		display->fbuf[i].red = 3000;
		display->fbuf[i].green = 3000;
		display->fbuf[i].blue = 3000;
		//whatever
		display->fbuf[i].alpha = 0;
		//HW2, z should initialized to MAXINT, z is GzDepth int
		display->fbuf[i].z = 2147483647;

	}
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	if (display == NULL) return GZ_FAILURE;
	
	int xs = static_cast<int>(display->xres);
	int ys = static_cast<int>(display->yres);
//	int size = static_cast<int>(display->xres * display->yres);
	int index = ARRAY(i, j);
	
	//bounds check, the biggest size of short is 4095

	if (i < 0 || i >= xs || j < 0 || j >= ys) {
		return GZ_FAILURE;
	}
	if (r < 0) r = 0;
	if (r > 4095) r = 4095;
	if (g < 0) g = 0;
	if (g > 4095) g = 4095;
	if (b < 0) b = 0;
	if (b > 4095) b = 4095;

	//put values
	display->fbuf[index].red = r;
	display->fbuf[index].green = g;
	display->fbuf[index].blue = b;
	display->fbuf[index].alpha = a;
	display->fbuf[index].z = z;

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	if (display == NULL) return GZ_FAILURE;

	int xs = static_cast<int>(display->xres);
	int ys = static_cast<int>(display->yres);
	int size = static_cast<int>(display->xres * display->yres);
	int index = ARRAY(i, j);

	//bounds check
	if (i < 0 || i >= xs || j < 0 || j >= ys) {
		return GZ_FAILURE;
	}
	//get values
	GzPixel curpixel = display->fbuf[index];
	*r = curpixel.red;
	*g = curpixel.green;
	*b = curpixel.blue;
	*a = curpixel.alpha;
	*z = curpixel.z;

	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	if (outfile == NULL) return GZ_FAILURE;
	if (display == NULL) return GZ_FAILURE;
	//file pointer
	FILE* f1 = outfile;
	//head of the file
	int xs = static_cast<int>(display->xres);
	int ys = static_cast<int>(display->yres);
	fprintf(f1, "P6 %d %d 255\n", xs, ys);

	//buffer order is RGB
	//Drop LS 4-bits by right-shifting and then use low byte of GzIntensity value
	for (int i = 0; i < xs*ys; i++) {
		GzPixel curpixel = display->fbuf[i];
		//requires conversion of GzIntensity to 8-bit rgb component
		//unsigned char 0-255 for color
		unsigned char p_r = curpixel.red >> 4;
		unsigned char p_g = curpixel.green >> 4;
		unsigned char p_b = curpixel.blue >> 4;
		fprintf(f1, "%c%c%c", p_r, p_g, p_b);
	}
	//we shouldn't close here
//	fclose(f1);
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	if (framebuffer == NULL) return GZ_FAILURE;
	if (display == NULL) return GZ_FAILURE;
	
	int size = static_cast<int>(display->xres * display->yres);

	for (int i = 0; i < size; i++) {
		GzPixel curpixel = display->fbuf[i];
		//requires conversion of GzIntensity to 8-bit rgb component
		//unsigned char 0-255 for color
		unsigned char p_r = curpixel.red >> 4;
		unsigned char p_g = curpixel.green >> 4;
		unsigned char p_b = curpixel.blue >> 4;

		framebuffer[i * 3 + 0] = p_b;
		framebuffer[i * 3 + 1] = p_g;
		framebuffer[i * 3 + 2] = p_r;

	}
	
	return GZ_SUCCESS;
}