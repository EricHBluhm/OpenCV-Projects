#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//COLOR DETECTION from webcam

Mat imgHSV, mask, imgColor;

//he found this, can get from trackbars
int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;

VideoCapture cap(0);
Mat img;

void main() {

	//string path = "Resources/lambo.png";
	

	//cvtColor(img, imgHSV, COLOR_BGR2HSV); // convert color of image, (input image, output image, argument)


	//to get the lower and upper values
	//creates a way to find in real time with track bars
	namedWindow("Trackbars", (640, 200));
	createTrackbar("Hue Min", "Trackbars", &hmin, 179); //creates trackbar(trackbar name, what window it is in, address of value, max value for hue is 179)  
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);//max 255 for rest
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);
	//start with mins at 0 and the max at max value
	//get the color you want to be white

	while (true)
	{
		cap.read(img);

		cvtColor(img, imgHSV, COLOR_BGR2HSV);

		Scalar lower(hmin, smin, vmin); //(hue min, saturation min, value min )
		Scalar upper(hmax, smax, vmax); //(hue min, saturation min, value min )
		//to collect color //(input, lower limit color, upper limit color, output);
		inRange(imgHSV, lower, upper, mask);

		//outputs the current values for slider
		cout << hmin << "," << smin << "," << vmin << "," << hmax << "," << smax << "," << vmax << "," << endl;
		imshow("Image", img);
		//imshow("Image HSV", imgHSV);
		imshow("Image Mask", mask);
		waitKey(1);
	}




}