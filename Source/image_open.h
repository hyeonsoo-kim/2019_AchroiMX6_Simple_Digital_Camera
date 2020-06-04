#ifndef _IMG_OPEN
#define _IMG_OPEN

#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "v4l_wrapper.h"

using namespace cv;

typedef struct _m {
	int num;
	int seq;
	} mem_file;

int Kbhit (void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}
#ifdef TEST
int image_open(int mem_num, int seq) {
	char buffer[2000000];
	int fd, fb;
	fd = open("still.yuv", O_RDWR);
	fb = open("/dev/fb0", O_RDWR);
	read(fd, buffer, 2000000);
	return write(fb, buffer, 2000000);
}  

#else
int image_open (int mem_num, int seq)
{
	char *filename = (char*)malloc(15);
	sprintf(filename, "photo_%d_%d.png", mem_num, seq);

	fsl_v4l_out mydisplay;
	int ret;
	char *buffer;

	int width = 640;
	int height = 480;
	IplImage *image;


	ret = V4LWrapper_CreateOutputDisplay (&mydisplay, "/dev/fb0", NULL, width, height);
	if (ret == V4LWrapper_SUCCESS)
		printf ("OK\n");
	else
	{
		exit (0);
	}
    Mat target  = imread(filename);
    imwrite("temp.bmp", target);
    image = cvLoadImage("temp.bmp");
//	buffer = (char *) malloc (image->imageSize);
	buffer = (char*) image -> imageData;

	//while (!Kbhit())
	//{
		V4LWrapper_OutputDisplay (&mydisplay, buffer);
	//}


	//V4LWrapper_CloseCameraCapture (&mycamera);
	V4LWrapper_CloseOutputDisplay (&mydisplay);

	free (buffer);
	cvReleaseImage (&image);
	free(filename);

	return 0;
}
#endif
void* open_image_thread(void *args) {
	mem_file *f = (mem_file*) args;
	return (void*)image_open(f->num, f->seq);
	}

#endif
