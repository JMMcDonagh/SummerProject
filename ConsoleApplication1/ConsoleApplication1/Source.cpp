#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>

cv::Point2f Translation(const cv::Mat& SRMatInv, const cv::Mat& offset, const cv::Mat& position);

float Determinant(const float* const ptr);
cv::Mat Inverse(const cv::Mat& inMat);

cv::Mat ScaleMatrix(float scale);
cv::Mat RotationMatrix(float angle);
float Deg2Rad(float angle);

int main()
{
	cv::CascadeClassifier face_cascade;
	if (!face_cascade.load("C:/opencv/data/haarcascades/haarcascade_frontalface_default.xml"))
	{
		std::cout << "Couldn't load face_cascade" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	cv::Mat image = cv::imread("Data/face5.jpg", CV_LOAD_IMAGE_COLOR);
	if(!image.data)
	{
		std::cout << "Could not open or find the image"  << std::endl;		
		std::exit(EXIT_FAILURE);
    }

	cv::Mat image_gray;
	cv::cvtColor(image, image_gray, CV_BGR2GRAY);
	cv::equalizeHist(image_gray, image_gray);	
	
	cv::vector<cv::Rect> faces;	
	face_cascade.detectMultiScale(image_gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, cv::Size(24, 24));

	if(faces.size() == 0)
	{
		std::cout << "Could not find any Faces" << std::endl;		
		std::exit(EXIT_FAILURE);
	}

	cv::Rect face(0, 0, 0, 0);
	for(unsigned int i = 0; i < faces.size(); i++)
	{
		if(faces[i].height > face.height)
		{
			face = faces[i];
		}
	}
	cv::rectangle(image, face, cv::Scalar(255, 255, 0));	

	cv::namedWindow("myWindow", CV_WINDOW_AUTOSIZE);
	cv::imshow("myWindow", image);

	
	cv::Mat grayImage = image_gray(face).clone();	
	cv::Rect grayRect(0, 0, grayImage.cols, grayImage.rows);


	cv::namedWindow("grayWindow", CV_WINDOW_AUTOSIZE);
	cv::imshow("grayWindow", grayImage);

	cv::Mat imageResult = cv::Mat::zeros(grayImage.rows, grayImage.cols, grayImage.type());


	

	float tX = 10.0f;
	float tY = 40.0f;	
	cv::Mat offset = (cv::Mat_<float>(2, 1) <<  tX, tY);

	float scale = 1.3f;
	cv::Mat scaleMat = ScaleMatrix(scale);


	float rotZ = 30.0f;
	cv::Mat rotationMat = RotationMatrix(rotZ);

	cv::Mat SRMat = scaleMat * rotationMat;
	cv::Mat SRMatInv = Inverse(SRMat);
	
	uchar* grayImagePtr = grayImage.data;

	for(int y = 0; y < imageResult.rows; y++)
	{
		uchar* resultPtr = imageResult.ptr<uchar>(y);		

		for(int x = 0; x < imageResult.cols; x++)
		{
			cv::Point2f result = Translation(SRMatInv, offset, (cv::Mat_<float>(2, 1) <<  x, y));

			if(!grayRect.contains(result))
			{
				continue;
			}
			
			resultPtr[x] = grayImagePtr[grayImage.step * cvRound(result.y) + cvRound(result.x)];						
		}
	}	
	
	cv::namedWindow("Translated image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Translated image", imageResult);
		
	cv::waitKey(0);
	
	return 0;
}


float Determinant(const float* const ptr)
{
	return (ptr[0] * ptr[3]) - (ptr[1] * ptr[2]);	
}

cv::Mat Inverse(const cv::Mat& inMat)
{
	const float* const ptr = (float*)inMat.data;

	float determinant = Determinant(ptr);	
	if(determinant == 0)
	{
		return inMat;
	}

	determinant = 1.0f / determinant;
	cv::Mat retMat = (cv::Mat_<float>(2, 2) <<  ptr[3], -ptr[1],
											   -ptr[2],  ptr[0]);
	return determinant * retMat;
}


float Deg2Rad(float angle)
{
	return angle * (float)(CV_PI / 180.0f);
}

cv::Mat ScaleMatrix(float s)
{
	return (cv::Mat_<float>(2, 2) << s, 0,
									 0, s);
}

cv::Mat RotationMatrix(float angle)
{
	float theta = Deg2Rad(angle);
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);

	return (cv::Mat_<float>(2, 2) <<  cosTheta, -sinTheta,
		                              sinTheta,  cosTheta);
}

cv::Point2f Translation(const cv::Mat& SRMatInv, const cv::Mat& offset, const cv::Mat& position)
{
	cv::Mat resultMat = SRMatInv * (position - offset);

	return cv::Point2f(resultMat);	
}