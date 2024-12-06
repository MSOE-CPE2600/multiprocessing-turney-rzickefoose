/***************************************************************** 
* Filename: mandelmovie.h
* Description: Header file for mandelmovie.h
* Author: Rose Zickefoose
* Date: 11/21/2024
* Note (Compile Instructions): 
*****************************************************************/

#include "jpegrw.h"

void mandelmovie(int children, imgRawImage* img[50], double xmin, double xmax, double ymin, double ymax, int max, int threads);
int iterations_at_point( double x, double y, int max );
void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max, int threads);
int iteration_to_color( int iters, int max );