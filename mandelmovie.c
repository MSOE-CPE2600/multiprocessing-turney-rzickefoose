/***************************************************************** 
* Filename: 
* Description: 
* Author: Rose Zickefoose
* Date: 
* Note (Compile Instructions): 
*****************************************************************/

#include "mandelmovie.h"

#include <unistd.h>

void mandelmovie(int children, imgRawImage* img[50], double xmin, double xmax, double ymin, double ymax, int max) {
	if (children == 1) {
		int index = 0;
		for (int i = 0; i < 50; i++) {
			compute_image(img[index], xmin+index, xmax-index, ymin+index, ymax-index, max);
			index++;
		}
	} else {
		int pids[children - 1];
		get_pids(pids, children);
		int processes_finished = 0;
		int img_per_child = 50 / children;
		int index = 0;
		if (50 % children == 0) {
			while (processes_finished < children) {
				for (int i = 0; i < children - 1; i++) {
					if (pids[i] == 0) {
						for (int j = (img_per_child*processes_finished)+1; j < img_per_child*(processes_finished+1); j++) {
							compute_image(img[index], xmin+index, xmax-index, ymin+index, ymax-index, max);
							index++;
						}
						processes_finished++;
					} else if (pids[children - 2] > 0) { // checks the last child
						for (int j = (img_per_child*processes_finished)+1; j < img_per_child*(processes_finished+1); j++) {
							compute_image(img[index], xmin+index, xmax-index, ymin+index, ymax-index, max);
							index++;
						}
						processes_finished++;
					}
				}
			}
		} else {
			int remainder = 50 % children;
			while (processes_finished < children) {
				for (int i = 0; i < children - 1; i++) {
					if (pids[i] == 0) {
						for (int j = (img_per_child*processes_finished)+1; j < img_per_child*(processes_finished+1); j++) {
							compute_image(img[index], xmin+index, xmax-index, ymin+index, ymax-index, max);
							index++;
						}
						processes_finished++;
					} else if (pids[children - 2] > 0) { // checks the last child
						for (int j = (img_per_child*processes_finished)+1; j < img_per_child*(processes_finished+1) + remainder; j++) {
							if (index < 50) {
								compute_image(img[index], xmin+index, xmax-index, ymin+index, ymax-index, max);
								index++;
							}
						}
						processes_finished++;
					}
				}
			}
		}
	}
}

void get_pids(int pids[], int children) {
	// Forks to get 2 initial children
	pids[0] = fork();
	if (children > 2) {
		int pids_needed = children - 1;
		int pid_count = 1;
		while (pid_count < pids_needed) {
			if (pids[pid_count-1] > 0) {
				pids[pid_count] = fork();
				pid_count++;
			}
		}
	}
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max )
{
	int i,j;

	int width = img->width;
	int height = img->height;

	// For every pixel in the image...

	for(j=0;j<height;j++) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			setPixelCOLOR(img,i,j,iteration_to_color(iters,max));
		}
	}
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}
