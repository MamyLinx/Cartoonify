#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void cartoonifyImage(Mat src, Mat dst){
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	const int MEDIAN_BLUR_FILTER_SIZE = 7;
	medianBlur(gray, gray, MEDIAN_BLUR_FILTER_SIZE);
	Mat edges;
	const int LAPLACIAN_FILTER_SIZE = 5;
	Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE);
	Mat _mask;
	const int EDGES_THRESHOLD = 80;
	threshold(edges, _mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);
	
	Size _size = src.size();
	Size smallSize;
	smallSize.width = _size.width/2;
	smallSize.height = _size.height/2;
	Mat smallImg = Mat(smallSize, CV_8UC3);
	resize(src, smallImg, smallSize, 0, 0, INTER_LINEAR );
	
	Mat tmp = Mat(smallSize, CV_8UC3);
	int repetitions = 7;
	for (int i=0; i<repetitions; i++){
		int ksize = 9;
		double sigmaColor = 9;
		double sigmaSpace = 7;
		bilateralFilter(smallImg, tmp, ksize, sigmaColor, sigmaSpace);
		bilateralFilter(tmp, smallImg, ksize, sigmaColor, sigmaSpace);
	}
	Mat bigImg;
	resize(smallImg, bigImg, _size, 0, 0, INTER_LINEAR);
	dst.setTo(0);
	bigImg.copyTo(dst, _mask);
}

int main(int argc, char **argv)
{
	int cameraNumber = 0;
	if (argc > 1)
		cameraNumber = atoi(argv[1]);
	//Get access to the camera
	VideoCapture camera;
	camera.open(cameraNumber);
	if (!camera.isOpened()){
		cerr << "ERROR: Could not access the camera or video" << endl;
		exit(1);
	}
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	while (true){
		//Grab next camera frame.
		Mat cameraFrame;
		camera >> cameraFrame;
		if (cameraFrame.empty()){
			cerr << "ERROR: Couldn't grab a camera frame." << endl;
			exit(1);
		}
		Mat displayedFrame(cameraFrame.size(), CV_8UC3);
		cartoonifyImage(cameraFrame, displayedFrame);
		imshow("Cartoonifier", displayedFrame);
		
		if(char(waitKey(1)) == 'q') 
			break;
	}
	destroyAllWindows();
	return 0;
}
