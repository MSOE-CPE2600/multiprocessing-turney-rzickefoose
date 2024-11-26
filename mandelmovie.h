/***************************************************************** 
* Filename: 
* Description: 
* Author: Rose Zickefoose
* Date: 
* Note (Compile Instructions): 
*****************************************************************/

#include "jpegrw.h"

void mandelmovie(int children, imgRawImage* img[50], double xmin, double xmax, double ymin, double ymax, int max);
void get_pids(int pids[], int children);
int iterations_at_point( double x, double y, int max );
void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max );
int iteration_to_color( int iters, int max );