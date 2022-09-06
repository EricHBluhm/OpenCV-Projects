#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;



void main() {

	VideoCapture cap(0); //amount of cameras. if only one put 0, its the camera id
	Mat img; //read image from the path 

	CascadeClassifier imageCascade;
	imageCascade.load("Resources/haarcascade_russian_plate_number.xml");

	if (imageCascade.empty()) { cout << "XML file not loaded" << endl; }

	//detect rImage using bounding boxes

	vector<Rect> rImage;

	while (true) 
	{
		cap.read(img); //reads images (webcam)

		imageCascade.detectMultiScale(img, rImage, 1.1, 10);

		//iterate through all rImage detected
		for (int i = 0; i < rImage.size(); i++)
		{
			
			Mat imgCrop = img(rImage[i]);//crop the thing we are detecing
			//imshow(to_string(i), imgCrop); //displays cropped image live in new windows, the to_sting makes i a string for the window name
			
			//save found image in a folder: can also
			imwrite("Resources/Plates/" + to_string(i) + ".png", img); //writes file to folder with name
			
			rectangle(img, rImage[i].tl(), rImage[i].br(), Scalar(255, 0, 255), 1);
		}


		imshow("Image", img);
		waitKey(1);
	}

}