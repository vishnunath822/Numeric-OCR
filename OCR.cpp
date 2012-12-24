#pragma once
#include <opencv2/core/core.hpp>
#include<opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "preprocess.h"

using namespace std;
using namespace cv;

IplImage* img;
IplImage* buff;
int dr, l_x, l_y,r=0;

void draw (int x, int y)
{
	
	cvCircle(img, cvPoint(x,y), r, CV_RGB(0,0,0), -1,4,0);
	buff = cvCloneImage(img);
	cvShowImage("Demo", buff);
}

void drawcursor(int x, int y)
{
	buff = cvCloneImage(img);
	cvCircle(buff, cvPoint(x,y), r, CV_RGB(0,0,0), 1, 4, 0);
}

void on_mouse( int event, int x, int y, int flags, void* param )
{
	l_x=x;
	l_y=y;
	drawcursor(x,y);
    if(CV_EVENT_LBUTTONDOWN == event)
        {
			dr=1;
			draw(x,y);
		}
    else if(CV_EVENT_LBUTTONUP == event)
		{
			dr=0;
		}
	else if(CV_EVENT_MOUSEMOVE == event  &&  flags & CV_EVENT_FLAG_LBUTTON)
		{
			if(dr==1)
				draw(x,y);
		}
}

int main( int argc, char* argv[])
{
	dr=0;
	r=10;
	l_x=l_y=0;
	img=cvCreateImage(cvSize(128,128),IPL_DEPTH_8U,1);
	cvSet(img, CV_RGB(255,255,255),NULL);
	buff=cvCloneImage(img);
	cvNamedWindow( "Demo", 0 );
	cvResizeWindow("Demo", 128,128);
	cvSetMouseCallback("Demo",&on_mouse, 0 );
	

	cout<<"OCR Number classification. A project by Vishnu Nath. \n"<<endl;
	cout<<"***************************************************************"<<endl;
	cout<<"Controls are as follows:"<<endl;
	cout<<"1. c - Classify the drawn image"<<endl;
	cout<<"2. r - Clear the whiteboard"<<endl;
	cout<<"3. + - Increase the pointer size"<<endl;
	cout<<"4. - - Decrease the pointer size"<<endl;
	cout<<"5. ESC - Quit the program"<<endl;
	cout<<"***************************************************************"<<endl;
	OCR ocr;
    for(;;)
    {
			int c;
			cvShowImage( "Demo", buff );
			c = cvWaitKey(10);
			if( (char) c == 27 )
				break;
			if( (char)c=='c')
			{
				ocr.classify(img, 1);
			}
			if( (char)c== 'r')
			{
				cvSet(img, cvRealScalar(255),NULL);
				drawcursor(l_x,l_y);
			}
			if( (char) c== '+' )
			{
				r++;
				drawcursor(l_x,l_y);
			}
			if( ((char)c== '-') && (r>1))
			{
				r--;
				drawcursor(l_x,l_y);
			}
    }

    cvDestroyWindow("Demo");
	return 0;
    
}