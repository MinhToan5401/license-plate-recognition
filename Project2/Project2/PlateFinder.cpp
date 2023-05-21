#include "PlateFinder.h"

using namespace cv;

CvScalar RED = cvScalar(0, 0, 255); 
CvScalar GREEN = cvScalar(0, 255, 0);
CvScalar BLUE = cvScalar(255, 0, 0);

PlateFinder::PlateFinder(void)
{
	S1 = cvCreateStructuringElementEx(3, 1, 1, 0, CV_SHAPE_RECT, NULL);
	S2 = cvCreateStructuringElementEx(3, 1, 1, 0, CV_SHAPE_RECT, NULL);
	plate = NULL;
}

PlateFinder::~PlateFinder(void)
{
	if (plate)
	{
		S1 = NULL;
		S2 = NULL;
		plate = NULL;
	}
}

void PlateFinder::ImageRestoration(IplImage* src)	//ham tien xu li anh
{
	int w = src->width;
	int h = src->height;

	IplImage* mImg = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);	//Anh su dung cho bien doi hinh thai hoc, khi resize anh mot so chi tiet se bien mat
	IplImage* src_Pyrdown = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);	//Khac phuc mot so chi tiet tu bien mat, giu nguyen cac diem anh quan trong
	IplImage* tmp = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);
	IplImage* thresholed = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);	//nhi phan nguong
	IplImage* mini_thresh = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);	//nhi phan nguong
	IplImage* dst = cvCreateImage(cvSize(w / 2, h / 2), IPL_DEPTH_8U, 1);	//anh da lam ro vung bien so

	cvPyrDown(src, src_Pyrdown);

	cvMorphologyEx(src_Pyrdown, mImg, tmp, S2, CV_MOP_BLACKHAT);
	cvNormalize(mImg, mImg, 0, 255, CV_MINMAX);

	//Nhi phan hoa anh mImg
	cvThreshold(mImg, thresholed, (int)10 * cvAvg(mImg).val[0], 255, CV_THRESH_BINARY);
	cvZero(dst);
	cvCopy(thresholed, mini_thresh);

	//Su dung cua so truot hinh chu nhat size 8x16 truot tren toan bo anh de loc ra bien so

	//cvShowImage("mini_thresh1", mini_thresh);
	int cnt;
	int nonZero1, nonZero2, nonZero3, nonZero4;
	CvRect rect;
	for (int i = 0; i < mini_thresh->width - 32; i += 4)
	{
		for (int j = 0; j < mini_thresh->height - 16; j += 4)
		{
			rect = cvRect(i, j, 16, 8);
			cvSetImageROI(mini_thresh, rect);	//ROI = Region of Interest
			nonZero1 = cvCountNonZero(mini_thresh);
			cvResetImageROI(mini_thresh);

			rect = cvRect(i+16, j, 16, 8);
			cvSetImageROI(mini_thresh, rect);	//ROI = Region of Interest
			nonZero2 = cvCountNonZero(mini_thresh);
			cvResetImageROI(mini_thresh);

			rect = cvRect(i, j+8, 16, 8);
			cvSetImageROI(mini_thresh, rect);	//ROI = Region of Interest
			nonZero3 = cvCountNonZero(mini_thresh);
			cvResetImageROI(mini_thresh);

			rect = cvRect(i+16, j+8, 16, 8);
			cvSetImageROI(mini_thresh, rect);	//ROI = Region of Interest
			nonZero4 = cvCountNonZero(mini_thresh);
			cvResetImageROI(mini_thresh);

			cnt = 0;
			if (nonZero1 > 15)	cnt++;
			if (nonZero2 > 15)	cnt++;
			if (nonZero3 > 15)	cnt++;
			if (nonZero4 > 15)	cnt++;

			if (cnt > 2)
			{
				rect = cvRect(i, j, 32, 16);
				cvSetImageROI(dst, rect);
				cvSetImageROI(mini_thresh, rect);
				cvCopy(mini_thresh, dst);
				cvResetImageROI(dst);
				cvResetImageROI(mini_thresh);
			}
			
		}
	}

	//tay mo di nhung vung anh khong can thiet
	IplImage* dst_clone = cvCloneImage(dst);

	cvDilate(dst, dst, NULL, 2);
	cvErode(dst, dst, NULL, 2);
	cvDilate(dst, dst, S1, 9);
	cvErode(dst, dst, S1, 10);
	cvDilate(dst, dst);

	/*cvShowImage("Source", src);
	cvShowImage("mImg", mImg);
	cvShowImage("mini_thresh", mini_thresh);
	cvShowImage("dst_truoc_xuly", dst_clone);
	cvShowImage("dst_sau_xuly", dst);*/

	cvPyrUp(dst, src);	//giai phong bo nho de khong bi treo vi dang code c++

	cvReleaseImage(&mini_thresh);
	cvReleaseImage(&mImg);
	cvReleaseImage(&tmp);
	cvReleaseImage(&dst);
	cvReleaseImage(&src_Pyrdown);
	cvReleaseImage(&thresholed);
	cvReleaseImage(&dst_clone);

}

IplImage* PlateFinder::FindPlate(IplImage *src) //The IplImage* structure contains information about the image such as its size, number of channels, and pixel data
//The IplImage* pointer is used to reference an instance of the IplImage structure.
{
	//tim duong bao cua cac vung trang
	IplImage* plate;
	IplImage* contourImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);	//anh tim contour
	IplImage* grayImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);	//anh xam
	cvCvtColor(src, grayImg, CV_RGB2GRAY);

	IplImage* cloneImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);	//IPL_DEPTH_8U: Each pixel in the image is represented by a single byte (8 bits) with a range of values from 0 to 255.
	cloneImg = cvCloneImage(src);

	//tien xu ly anh
	cvCopy(grayImg, contourImg);	//copy tu grayImg sang cloneImg
	cvNormalize(contourImg, contourImg, 0, 255, CV_MINMAX);	//xu ly nhieu, CV_MINMAX is used to normalize the image to the range [a,b]
	ImageRestoration(contourImg);

	IplImage* rectImg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	cvMerge(contourImg, contourImg, contourImg, NULL, rectImg);	//tron anh

	//Tim contour cua buc anh - bat di bat dich 3 cau lenh
	CvMemStorage* storagePlate = cvCreateMemStorage(0);
	CvSeq* contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storagePlate);
	cvFindContours(contourImg, storagePlate, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	int xmin, ymin, xmax, ymax, w, h, s, r; //toa do, chieu dai, rong, dien tich hinh chu nhat
	int count;	//tren contour do co bao nhieu pixel
	double ratio;	//ty le chieu rong tren chieu cao
	CvRect rectPlate;	//used to represent a rectangle

	//luu lai cac anh co kha nang la bien so
	IplImage** plateArr = new IplImage * [5];	//mang con tro 2 chieu
	int j = 0;
	for (int i = 0; i < 5; i++)
	{
		plateArr[i] = NULL;
	}
	 
	while (contours)
	{
		count = contours->total;
		CvPoint* PointArray = new CvPoint[count];
		cvCvtSeqToArray(contours, PointArray, CV_WHOLE_SEQ);

		for (int i = 0; i < count; i++)
		{
			if (i == 0)
			{
				xmin = xmax = PointArray[i].x;
				ymin = ymax = PointArray[i].y;
			}
			if (PointArray[i].x > xmax)
			{
				xmax = PointArray[i].x;
			}
			if (PointArray[i].x < xmin)
			{
				xmin = PointArray[i].x;
			}
			if (PointArray[i].y > ymax)
			{
				ymax = PointArray[i].y;
			}
			if(PointArray[i].y < ymin)
			{
				ymin = PointArray[i].y;
			}
		}
		w = xmax - xmin;
		h = ymax - ymin;
		s = w * h;

		cvRectangle(rectImg, cvPoint(xmin, ymin), cvPoint(xmax, ymax), RED);

		//loai bo nhung hinh chu nhat qua nho hoac qua to so voi anh goc
		if (s != 0)
		{
			r = (contourImg->height * contourImg->width) / s;	//ti le cua anh voi hcn contour
		}
		else
		{
			r = 1000;
		}
		if (w == 0 && h == 0)
		{
			ratio = 0;
		}
		else
		{
			ratio = (double)w / h;
		}
		if (r > 30 && r < 270)
		{
			//ve hcv mau xanh la
			cvRectangle(rectImg, cvPoint(xmin, ymin), cvPoint(xmax, ymax), GREEN);

			if (ratio > 2.6 && ratio < 7)
			{
				cvRectangle(rectImg, cvPoint(xmin, ymin), cvPoint(xmax, ymax), BLUE);
				if (w > 80 && w < 250 && h > 25 && h < 150)
				{
					rectPlate = cvRect(xmin, ymin, w, h);

					cvRectangle(cloneImg, cvPoint(rectPlate.x, rectPlate.y), cvPoint(rectPlate.x + rectPlate.width, rectPlate.y + rectPlate.height), RED, 3);

					//cat bien so
					plate = cvCreateImage(cvSize(rectPlate.width, rectPlate.height), IPL_DEPTH_8U, 3);
					cvSetImageROI(src, rectPlate);// The function allows you to define a rectangular area of an image where you want to perform further processing operations.
					cvCopy(src, plate, NULL);
					cvResetImageROI(src);
					cvShowImage("plate", plate);


					//luu vao mang cac bien so plateArr
					int cnt = CountCharacter(plate);
					if (cnt >= 5)
					{
						plateArr[j] = plate;
						j++;
					}
				}
			}
		}
		delete[]PointArray;

		contours = contours->h_next;	//thoat khoi vong lap
	}

	//sap xep
	if (plateArr[0])
	{
		int w = plateArr[0]->width;

		int flag;
		for (int i = 1; i < 4; i++)
		{
			if (plateArr[i] && plateArr[i]->width < w)
			{
				flag = i;
			}
		}
		plateArr[0] = plateArr[flag];
	}

	//cvShowImage("cloneImg", cloneImg);
	//cvShowImage("rectImg", rectImg);
	//cvShowImage("plate", plateArr[0]);

	cvReleaseImage(&contourImg);
	cvReleaseImage(&rectImg);
	cvReleaseImage(&plate);

	return plateArr[0];
}

int PlateFinder::CountCharacter(IplImage *plate)
{
	int cnt = 0;
	IplImage *resizeImg, *grayImg, *binaryImg;

	resizeImg = cvCreateImage (cvSize(408, 70), IPL_DEPTH_8U, 3);
	grayImg = cvCreateImage (cvSize(408, 70), IPL_DEPTH_8U, 1);
	binaryImg = cvCreateImage (cvSize(408, 70), IPL_DEPTH_8U, 1);

	cvResize (plate, resizeImg);
	cvCvtColor (resizeImg, grayImg, CV_RGB2GRAY);
	cvAdaptiveThreshold (grayImg, binaryImg, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 13, 2);

	//cvShowImage("binaryImg", binaryImg);

	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *contours = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);
	cvFindContours(binaryImg, storage, &contours);

	//cvShowImage("contours", binaryImg);

	//CvSeq* contours = 0;
	//cvFindContours(binaryImg, storage, &contours);

	while (contours)
	{
		CvRect rect = cvBoundingRect(contours);

		if (rect.width > 15 && rect.width < 50 && rect.height > 40 && rect.height < 65 && rect.height * rect.width > 1000)
		{
			cvRectangle(resizeImg, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), GREEN, 2);
			cnt++;
		}
		contours = contours->h_next;
	}

	//cvShowImage("resizeImg", resizeImg);	//khoanh vung ky tu

	return cnt;
}