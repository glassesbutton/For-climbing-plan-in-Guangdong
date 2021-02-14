#include "opencv2/opencv.hpp"
#include <iostream>
#include<iostream>
#include<tesseract/baseapi.h>
//#include<baseapi.h>
/*#include<allheaders.h>
#include<resultiterator.h>
#include<blobs.h>*/
#include<leptonica/allheaders.h>
#include<tesseract/resultiterator.h>
//#include<>
#include<string.h>
#include<Windows.h>
#include<wchar.h>

using namespace cv;
using namespace std;

wchar_t * Utf_8ToUnicode(char* szU8);
char* UnicodeToAnsi(const wchar_t* szStr);
PIX* cvtMat2PIX(Mat imgGray);

int main()
{
	VideoCapture inputVideo(0);
	if (!inputVideo.isOpened())
	{
		cout << "Could not open the input video: " << endl;
		return -1;
	}
	Mat frame;
	Mat frameCalibration;

	inputVideo >> frame;

	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	cameraMatrix.at<double>(0, 0) = 88.4608224769439;
	cameraMatrix.at<double>(0, 1) = 0;
	cameraMatrix.at<double>(0, 2) = 303.636211539177;
	cameraMatrix.at<double>(1, 1) = 88.3553214346056;
	cameraMatrix.at<double>(1, 2) = 252.182612850320;

	Mat distCoeffs = Mat::zeros(4, 1, CV_64F);
	distCoeffs.at<double>(0, 0) = -0.0119062403098344;
	distCoeffs.at<double>(1, 0) = 0.000191081505028223;
	distCoeffs.at<double>(2, 0) = 0;
	distCoeffs.at<double>(3, 0) = 0;

	Mat view, rview, map1, map2, base_MOG2, base_KNN, element, threshold_frame;
	Size imageSize;
	imageSize = frame.size();

	initUndistortRectifyMap(cameraMatrix, distCoeffs, noArray(),
		getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 0),
		imageSize, CV_32FC1, map1, map2);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

	//	Ptr <BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();

	vector<vector<cv::Point>> contours_point;
	vector<cv::Vec4i> contour_tree;

	bool flag = 1;

	Mat grey, edges, contours = Mat::zeros(frameCalibration.rows, frameCalibration.cols, CV_8UC3);
	RotatedRect  rect_data;
	Point2f rect_point[4];

	vector<Rect> boundRect;
	vector<cv::Point> contour_box;
	RotatedRect box;
	Point2f rect[4];

	PIX* image_1;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
/*	char *text=NULL;
	char *result=NULL;
	wchar_t *temp=NULL;*/

	while (1) 
	{
		inputVideo >> frame;              
		if (frame.empty()) break;         
		remap(frame, frameCalibration, map1, map2, INTER_LINEAR);

		cvtColor(frameCalibration, grey, COLOR_BGR2GRAY);

		threshold(grey, threshold_frame, 220, 255, THRESH_BINARY);


		GaussianBlur(threshold_frame, threshold_frame, Size(5, 5), 0, 0);

		imshow("GaussianBlur", threshold_frame);

		image_1 = cvtMat2PIX(threshold_frame);

		//IplImage *iplimg = cvLoadImage("E:\\Program Files (x86)\\vc\\3.png");
		api->Init("C:\\Program Files (x86)\\tesseract\\bin\\tessdata", "glasses");

		api->SetImage(image_1);

		char *text = api->GetUTF8Text();
		wchar_t *temp = Utf_8ToUnicode(text);
		char *result = UnicodeToAnsi(temp);

		//char *imagenation =NULL;
		//memcpy(imagenation, result, sizeof(result));

		//if(imagenation!=result)
		std::cout << "获得的结果为： " << result << std::endl;
		for (int i = 0; i < 500; i++);
	//	Canny(threshold_frame, edges, 100, 150, 3, true);

		imshow("fuck", frameCalibration);

		delete[]text;
		delete[]temp;
		delete[]result;
	//	delete[]imagenation;

		char key = waitKey(1);
		if (key == 27 || key == 'q' || key == 'Q')break;
	}
/*	delete[]text;
	delete[]temp;
	delete[]result;*/
	return 0;
}

char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];

	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);

	return pResult;

}

wchar_t * Utf_8ToUnicode(char* szU8)
{
	//UTF8 to Unicode
	//由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式

	//预转换，得到所需空间的大小
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	//最后加上'\0'
	wszString[wcsLen] = '\0';
	return wszString;
}

PIX* cvtMat2PIX(Mat imgGray)
{
	int cols = imgGray.cols;
	int rows = imgGray.rows;

	PIX *pixS = pixCreate(cols, rows, 8);

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			pixSetPixel(pixS, j, i, (l_uint32)imgGray.at<uchar>(i, j));
	return pixS;
}