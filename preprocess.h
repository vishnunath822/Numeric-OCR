#pragma once

#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv\ml.h>
#include<stdio.h>

IplImage preprocess(IplImage*, int, int);

class OCR
{
public:
	CvKNearest* knn;
	static const int k = 10;
	char file[255];
	int trains;
	int classes;
	CvMat* train_data;
	CvMat* train_class;
	int size;
	void getdata();
	void train();
	void test();
	OCR();
	float classify(IplImage*, int);
};


void OCR::getdata()
{
	IplImage* src;
	IplImage* image;
	IplImage dst;
	CvMat row, data, row_head;
	CvMat* row1;
	char file[255];
	
	for (auto i = 0; i < classes; i++)
	{
		for (auto j = 0; j < trains; j++)
		{
			if (j<10)
				sprintf(file,"%s%d/%d0%d.pbm",file, i, i , j);
			else
				sprintf(file,"%s%d/%d%d.pbm",file, i, i , j);
		src = cvLoadImage(file, 0);
		if (!src)
			cout<<"Cannot open the image"<<endl;
		dst = preprocess(src, size, size);
		cvGetRow(train_class, &row, i * trains + j);
		cvSet(&row, cvRealScalar(i));

		cvGetRow(train_data, &row, i*trains + j);

		image = cvCreateImage(cvSize(size, size), IPL_DEPTH_32F, 1);
		cvConvertScale(&dst, image, 0.004, 0);
		cvGetSubRect(image, &data, cvRect(0,0, size, size));
		
		row1 = cvReshape (&data, &row_head, 0, 1);
		cvCopy(row1, &row, NULL);
		}
	}
}

float OCR::classify(IplImage* img, int sresult)
{
	IplImage* temp;
	IplImage dst;
	CvMat data;
	CvMat* nearest = cvCreateMat(1,k, CV_32FC1);
	CvMat row, *row1;
	float ans=0;
	float result = 0;
	int acc = 0;

	dst = preprocess (img, size, size);
	temp = cvCreateImage(cvSize(size, size), IPL_DEPTH_32F, 1);
	cvConvertScale(&dst, temp, 0.004,0);
	cvGetSubRect(temp, &data, cvRect(0,0,size, size));

	row1 = cvReshape(&data, &row, 0, 1);
	result = knn->find_nearest(row1, k, 0,0,nearest, 0);
	
	for (auto i = 0; i < k; i++)
	{
		if (nearest->data.fl[i] == result)
		{
			acc +=1;
		}
	}

	ans = 100 * ((float) acc/ (float)k);
	if (sresult == 1)
	{
		cout<<"| "<<result<<" | "<<ans<<" | "<<acc<<" | "<<k<<" | "<<endl;
		cout<<"-----------------------------------------------------------"<<endl;
	}
	return result;

}

void OCR::train()
{
	knn = new CvKNearest(train_data, train_class, 0, false, k);
}


void OCR::test()
{
	IplImage* src;
	IplImage dst;
	CvMat row, data;
	char file[255];
	int error=0;
	int total_error=0;
	int test_count = 0;

	for (auto i = 0; i<classes; i++)
	{
		for (auto j=50; j < 50 + trains; j++)
		{
			sprintf(file,"%s%d/%d%d.pbm",file, i, i , j);
			src = cvLoadImage(file,0);
			if (!src)
				cout<<"There has been an error in loading the image."<<endl;
		
			dst = preprocess(src, size, size);
			float r = classify(&dst, 0);
			int x = (int)r;

			if (x != i)
				error +=1;

			test_count +=1;
		}
	}

	total_error = 100 * (float)error / (float)test_count;
	//cout<<"The total error = "<<total_error<<endl;
}

OCR::OCR()
{
	sprintf(file, "C:\\basicOCR-master\\basicOCR-master\\OCR\\");
	train_data = cvCreateMat(500, 1600, CV_32FC1);
	train_class = cvCreateMat(500, 1, CV_32FC1);

	getdata();
	train();
	test();

	cout<<"-----------------------------------------------"<<endl;
	cout<<"|Class		|Precision		|Accuracy		  "<<endl;
	cout<<"-----------------------------------------------"<<endl;

}

void findx(IplImage* src, int& min, int& max)
{
	int minfind=0;
	CvMat data;
	CvScalar maxVal = cvRealScalar(src->width * 255);
	CvScalar val = cvRealScalar(0);

	for (auto i=0; i<src->width; i++)
	{
		cvGetCol(src, &data, i);
		val = cvSum(&data);

		if (val.val[0] < maxVal.val[0])
		{
			max = i;
			if (!minfind)
			{
				min = i;
				minfind = i;
			}
		}

	}
}

void findy(IplImage* src, int& min, int& max)
{
	int minfind=0;
	CvMat data;
	CvScalar maxVal = cvRealScalar(src->width * 255);
	CvScalar val = cvRealScalar(0);

	for (auto i=0; i<src->height; i++)
	{
		cvGetRow(src, &data, i);
		val = cvSum(&data);

		if (val.val[0] < maxVal.val[0])
		{
			max = i;
			if (!minfind)
			{
				min = i;
				minfind = i;
			}
		}

	}
}


CvRect makebox(IplImage* src)
{
	CvRect img;
	int xmin, xmax, ymin, ymax = 0;
	
	findx(src, xmin, xmax);
	findy(src, ymin, ymax);

	int xbuff = xmax - xmin;
	int ybuff = ymax - ymin;

	img = cvRect(xmin, ymin, xbuff, ybuff);
	return img;
}


IplImage preprocess (IplImage* src, int width, int height)
{
	CvMat data;
	CvRect bound;
	
	bound = makebox(src);

	cvGetSubRect(src, &data, cvRect (bound.x, bound.y, bound.width, bound.height));
	int size = 0;
	
	if (bound.width > bound.height)
		size = bound.width;
	else if (bound.width < bound.height)
		size = bound.height;
	else //just to be really sure!
		size = bound.height;
	int x = floor((double)(size-bound.width)/2);
	int y = floor((double)(size-bound.height)/2);

	IplImage* result = cvCreateImage(cvSize(size, size), 8, 1);
	cvSet(result, CV_RGB(255,255,255), NULL);
	CvMat datan;
	cvGetSubRect(result, &datan, cvRect(x,y,bound.width, bound.height));
	cvCopy(&data, &datan, NULL);

	IplImage* temp_buffer;
	temp_buffer = cvCreateImage(cvSize(width, height), 8, 1);
	cvResize(result, temp_buffer, CV_INTER_NN);
	return *temp_buffer;	
	 
}

