#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//Document Scan

Mat imgOriginal, imgGray, imgCanny, imgThresh, imgDil, imgErode, imgBlur, imgWarp, imgCrop;
vector<Point> initialPoints, docPoints;

float w = 420, h = 596; //width and height of paper * 2

//gets outlines of image so we can find the document's outline
Mat preProcess(Mat img)
{
	//convert images into grayscale
	cvtColor(img, imgGray, COLOR_BGR2GRAY); 
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0); 

	//edge detection //common practice is to blur the image before doing edge detection
	Canny(imgBlur, imgCanny, 25, 75); //does edge detection, decrease numbers to get more edges

	//dilate/erode images //increases or decreases width of edge detection lines in Canny : 45
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); //creates a kernel to use with dialation, if we increase the Size() numbers more dilation, decrease is less dilation
	dilate(imgCanny, imgDil, kernel);
	//erode(imgDil, imgErode, kernel); //for erosion, can use same kernel or make own

	return imgDil;
}

vector<Point> getContours(Mat imgOriginal)
{
	vector<vector<Point>> contours; //stores contours
	vector<Vec4i> hierarchy; //Vec4i is opencv type that is 4 integers

	//gets contours
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2); //(input, contours, which conouts we want -1 is all, color, thicknes)

	//add filter to get rid of noise & determine shape
	//if area of some contours is too small and we don't want it, we can get rid of it
	//loop through all contours that we found:

	vector<vector<Point>> conPoly(contours.size()); //stores the edges
	vector<Rect> boundRectVec(contours.size());
	
	int maxArea = 0;
	
	vector<Point> biggest;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);

		if (area > 1000)
		{
			//find bounding box, which is max area the shape takes up in a rectangle

			float peri = arcLength(contours[i], true); // (contour, boolean if object is closed or not)

			//find corner points or number of curves & stores them in conPoly vector<vector>
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true); //(input, array to store points, random number (can use whatever), bool if closed);

			if (area > maxArea && conPoly[i].size() == 4)//checks to see if a larger area is found & if its a square/rectangle
			{
				//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 5); 
				biggest = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] }; //bigest rectangle found
				maxArea = area;
			}
	

			//rectangle(imgOriginal, boundRectVec[i].tl(), boundRectVec[i].br(), Scalar(0, 255, 0), 5); //draws bounding box on video
			//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2); //draws contours on video based on color detection

		}
	}

	return biggest;

}


void writePoints(vector<Point> points, Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal, points[i], 10, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 2, color, 2);
	}
}

//to get the location of the four corners and store them in the right order (tl,tr,bl,br)
vector<Point> reOrder(vector<Point> points)
{
	vector<Point> newPoints;
	vector<int> summedPoints, subtractPoints;

	for (int i = 0; i < 4; i++)
	{
		summedPoints.push_back(points[i].x + points[i].y);
		subtractPoints.push_back(points[i].x - points[i].y);
	}
	
	newPoints.push_back(points[min_element(summedPoints.begin(), summedPoints.end()) - summedPoints.begin()]); //tl
	newPoints.push_back(points[max_element(subtractPoints.begin(), subtractPoints.end()) - subtractPoints.begin()]); //tr
	newPoints.push_back(points[min_element(subtractPoints.begin(), subtractPoints.end()) - subtractPoints.begin()]); //bl
	newPoints.push_back(points[max_element(summedPoints.begin(), summedPoints.end()) - summedPoints.begin()]); //br

	return newPoints;

}

Mat getWarp(Mat img, vector<Point> points, float w, float h)
{
	Point2f src[4] = { points[0],points[1] ,points[2] ,points[3] }; //source point
	Point2f dst[4] = { { 0.0f, 0.0f}, {w, 0.0f}, {0.0f,h}, {w,h} }; //destination point // 0.0f is zero if float


	//transform
	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));

	return imgWarp;
}

void main() {
	
	string path = "Resources/paper.jpg";
	imgOriginal = imread(path); //read image from the path 
	//resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5); //to scale it down to make it easier to see


	//Preprocessing
	imgThresh = preProcess(imgOriginal);
	
	//Get contours - the biggest one
	initialPoints = getContours(imgThresh);
	//writePoints(initialPoints, Scalar(0, 0, 255));
	docPoints = reOrder(initialPoints);
	//writePoints(docPoints, Scalar(0, 255, 0));


	//warp image
	imgWarp = getWarp(imgOriginal, docPoints, w, h);

	//crop
	int cropVal = 5;
	Rect rec(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
	imgCrop = imgWarp(rec);

	imshow("Image", imgOriginal); //show image
	imshow("Image Dialation", imgThresh); //show image
	imshow("Image Warped", imgWarp); //show image
	imshow("Image Cropped", imgCrop); //show image
	waitKey(0); //makes it so it won't close the image until we do it manually

}