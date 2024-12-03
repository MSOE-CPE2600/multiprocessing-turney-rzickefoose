/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include "mandel.h"
#include <stdlib.h>
#include <string.h>

#include "mandelmovie.h"

int main( int argc, char *argv[] ) {
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	char *outfile_start = "mandel";
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int children = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:c:h"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
			break;
			case 'y':
				ycenter = atof(optarg);
			break;
			case 's':
				xscale = atof(optarg);
			break;
			case 'W':
				image_width = atoi(optarg);
			break;
			case 'H':
				image_height = atoi(optarg);
			break;
			case 'm':
				max = atoi(optarg);
			break;
			case 'o':
				outfile_start = optarg;
			break;
			case 'c':
				children = atoi(optarg);
			break;
			case 'h':
				show_help();
			exit(1);
			break;
		}
	}

	// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
	yscale = xscale / image_width * image_height;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n",xcenter,ycenter,xscale,yscale,max,outfile_start);

	// Create a raw image of the appropriate size.
	imgRawImage* img[50];
	for (int i = 0; i < 50; i++) {
		img[i] = initRawImage(image_width,image_height);
	}

	// Fill it with a black
	for (int i = 0; i < 50; i++) {
		setImageCOLOR(img[i],0);
	}

	/* THIS IS WHERE TO PUT MADEL MOVIE
	// Compute the Mandelbrot image
	compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max);
	*/
	mandelmovie(children, img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max);


	/* CHANGE TO FOR LOOP OF AN ARRAY OF IMAGES */
	// Save the image in the stated file.
	for (int i = 0; i < 50; i++) {
		char file_number[100];
		sprintf(file_number,"%d",i);
		char outfile[100];
		strcpy(outfile,outfile_start);
		strcat(outfile,file_number);
		strcat(outfile,".jpg");
		storeJpegImageFile(img[i],outfile);
	}

	// free the mallocs
	for (int i = 0; i < 50; i++) {
		freeRawImage(img[i]);
	}

	return 0;
}

// Show help message
void show_help() {
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
