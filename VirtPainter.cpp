#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


//webcam

VideoCapture cap(0); //amount of cameras. if only one put 0, its the camera id
Mat img;
vector<vector<int>> newPoints; //{{x position,y position,color value (0,1,2,...)},{}}


//values from ColorGrab : hmin , smin, vmin, hmax, smax ,vmax
vector<vector<int>> myColors{{89,147,132,107,255,255}, //blue
								{9,97,165,32,151,227}, //yellow
					};

//to show that we have detected the above colors
vector<Scalar> ColorValues{ {255,0,255}, //purple
							{0,255,0} }; //green

Point getContours(Mat imgDil)  
{
	vector<vector<Point>> contours; //stores contours
	vector<Vec4i> hierarchy; //Vec4i is opencv type that is 4 integers

	//gets contours
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2); //(input, contours, which conouts we want -1 is all, color, thicknes)

	//add filter to get rid of noise & determine shape
	//if area of some contours is too small and we don't want it, we can get rid of it
	//loop through all contours that we found:

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRectVec(contours.size());
	string objectType;
	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);

		if (area > 1000)
		{
			//find bounding box, which is max area the shape takes up in a rectangle

			float peri = arcLength(contours[i], true); // (contour, boolean if object is closed or not)

			//find corner points or number of curves 
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true); //(input, array to store points, random number (can use whatever), bool if closed);

			////get bounding box and (optional) put bounding box on image
			boundRectVec[i] = boundingRect(conPoly[i]); //gives bounding box around each contour, we store the values
			
			//get point where we want to draw the circle. Based on bounding box rectangle
			myPoint.x = boundRectVec[i].x + boundRectVec[i].width / 2;
			myPoint.y = boundRectVec[i].y;
			
			rectangle(img, boundRectVec[i].tl(), boundRectVec[i].br(), Scalar(0,255,0), 5); //draws bounding box on video
			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2); //draws contours on video based on color detection
			
		}
	}

	return myPoint;

}



vector<vector<int>> colorFinder(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV); //


	//for each of the colors we want to detect, we need to find a mask
	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]); //(hue min, saturation min, value min )
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]); //(hue min, saturation min, value min )
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);

		Point myPoint = getContours(mask); //gets the contours based on mask and prints them on image

		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x,myPoint.y,i }); //i is the color 
		}
	}

	return newPoints;
	
}


void drawCanvas(vector<vector<int>> newPoints, vector<Scalar> ColorValues)
{
	for (int i = 0; i < newPoints.size(); i++)
	{
		//follows the color and draws
		circle(img, Point(newPoints[i][0],newPoints[i][1]), 10, ColorValues[newPoints[i][2]], FILLED);
	}
}


void main() {
 

	while (true) 
	{
		cap.read(img); //reads images (webcam)
		newPoints = colorFinder(img);
		drawCanvas(newPoints, ColorValues);

		//mat is matrix data type from opencv to deal with the images, its a data type

		imshow("Image", img); //show image (here webcam)
		waitKey(1); //delays for 1 milisecond
	}


}