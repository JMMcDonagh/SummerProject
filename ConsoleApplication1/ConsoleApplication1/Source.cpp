#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>

int main()
{
	cv::CascadeClassifier face_cascade;
	if (!face_cascade.load("C:/opencv/data/haarcascades/haarcascade_frontalface_alt.xml"))
	{
		std::cout << "Couldn't load face_cascade" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	cv::Mat image = cv::imread("Data/face4.jpg", CV_LOAD_IMAGE_COLOR);
	if(!image.data)
	{
		std::cout << "Could not open or find the image"  << std::endl;		
		std::exit(EXIT_FAILURE);
    }

	cv::Mat image_gray;
	cv::cvtColor(image, image_gray, CV_BGR2GRAY);
	cv::equalizeHist(image_gray, image_gray);	
	
	cv::vector<cv::Rect> faces;	
	face_cascade.detectMultiScale(image_gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, cv::Size(0, 0), cv::Size(200, 200));

	for(unsigned int i = 0; i < faces.size(); i++)
	{
		cv::rectangle(image, faces[i], cv::Scalar(255, 255, 0));
	}

	cv::namedWindow("myWindow", CV_WINDOW_AUTOSIZE);
	cv::imshow("myWindow", image);
	
	cv::waitKey(0);

	return 0;
}