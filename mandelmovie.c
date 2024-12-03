/***************************************************************** 
* Filename: mandelmovie.c
* Description: Creates 50 images using multiprocessing
* Author: Rose Zickefoose
* Date: 11/21/2024
* Note (Compile Instructions): make
*****************************************************************/

#include "mandelmovie.h"
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void mandelmovie(int children, imgRawImage* img[50], double xmin, double xmax, double ymin, double ymax, int max) {
	if (children == 1) {
		int index = 0;
		for (int i = 0; i < 50; i++) {
			if (index < 50) {
				compute_image(img[index], xmin-(index), xmax+(index), ymin-(index), ymax+(index), max);
				printf("Image %d finished\n", index);
				index++;
			}
		}
	} else {
		// Creating Shared Memory for processes (for index)
		int shmid;
		key_t key = ftok("shmfile", 65);
		shmid = shmget(key, 1024, IPC_CREAT | 0666);
		if (shmid < 0) {
			perror("shmget");
			exit(1);
		}
		int *index_ptr = (int*)shmat(shmid, NULL, 0);
		memset(index_ptr, 0, 1024);

		// Creates Shared Memory with the image array
		imgRawImage** img_ptr;
		int fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
		if (fd == -1) {
			perror("shm_open");
			exit(1);
		}

		if (ftruncate(fd, 50 * sizeof(imgRawImage*)) < 0) {
			perror("ftruncate");
			exit(1);
		}

		img_ptr = (imgRawImage**)mmap(NULL, 50 * sizeof(imgRawImage*), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (img_ptr == MAP_FAILED) {
			perror("mmap");
			exit(1);
		}

		// Creating Semaphores for processes
		sem_t sem;
		sem_init(&sem, children, 1);

		// Creating Children processes
		int pids_needed = children - 1;
		int pid = fork();
		pids_needed--;
		while (pids_needed > children) { // gets the needed processes
			if (pid != 0) {
				fork();
				pids_needed--;
			}
		}

		memcpy(img_ptr, img, 50 * sizeof(imgRawImage*));

		// Creating images
		for (int i = 0; i < 50; i++) {
			if (*index_ptr < 50) {
				if (pid == 0) {
					sem_wait(&sem);
					compute_image(*(img_ptr+(*index_ptr)), xmin-(*index_ptr), xmax+(*index_ptr), ymin-(*index_ptr), ymax+(*index_ptr), max);
					sem_post(&sem);
					sem_wait(&sem);
					printf("Image %d finished\n", (*index_ptr)++);
					usleep(1000);
					sem_post(&sem);
				}
			}
		}

		memcpy(img, img_ptr, 50 * sizeof(imgRawImage*));

		// Detaches Shared Memory
		shmdt(index_ptr);
		munmap(img_ptr, 50 * sizeof(imgRawImage*));

		// Closes Shared Memory Object
		close(fd);

		// Removes Shared Memory
		shmctl(shmid, IPC_RMID, NULL);
		shm_unlink("/my_shared_memory");

		// Destroys the semaphore
		sem_destroy(&sem);
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