#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>


cv::Point2f Translation(const cv::Mat& SRMatInv, const cv::Point2f& offset, const cv::Point2f& position);

float Determinant(const cv::Mat& inMat);
cv::Mat Inverse(const cv::Mat& inMat);

cv::Mat ScaleMatrix(float scale);
cv::Mat RotationMatrix(float angle);
float Deg2Rad(float angle);

int main()
{	
	std::string imageFile = "Data/face1.jpg";
	cv::Mat imageOrginal = cv::imread(imageFile, CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat imageResult = cv::Mat::zeros(imageOrginal.rows, imageOrginal.cols, imageOrginal.type());
	
	cv::Rect rect(0, 0, imageOrginal.cols, imageOrginal.rows);

	
	
	float tX = 40.0f;
	float tY = 0.0f;
	cv::Point2f offset(tX, tY);

	float scale = 1.0f;
	cv::Mat scaleMat = ScaleMatrix(scale);

	float rotZ = 0.0f;
	cv::Mat rotationMat = RotationMatrix(rotZ);

	cv::Mat SRMat = scaleMat * rotationMat;
	cv::Mat SRMatInv = Inverse(SRMat);

	for(int y = 0; y < imageResult.rows; y++)
	{
		for(int x = 0; x < imageResult.cols; x++)
		{
			cv::Point2f result = Translation(SRMatInv, offset, cv::Point2f(x, y));

			if(!rect.contains(result))
			{
				continue;
			}
			else
			{				
				imageResult.at<unsigned char>(y , x) = imageOrginal.at<unsigned char>(result.y , result.x);
			}
		}
	}

	cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Source image", imageOrginal );

	cv::namedWindow("Translated image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Translated image", imageResult );

	cv::waitKey(0);

	return 0;
}

float Determinant(const cv::Mat& inMat)
{	
	return (inMat.at<float>(0 , 0) * inMat.at<float>(1 , 1)) - (inMat.at<float>(0 , 1) * inMat.at<float>(1 , 0));	
}

cv::Mat Inverse(const cv::Mat& inMat)
{
	float determinant = Determinant(inMat);
	float det;
	if(determinant == 0)
	{
		det = 1.0f;
	}
	else
	{
		det = 1.0f / determinant;
	}

	cv::Mat retMat = (cv::Mat_<float>(2, 2) <<  inMat.at<float>(1 , 1), -inMat.at<float>(0 , 1),
											   -inMat.at<float>(1 , 0),  inMat.at<float>(0 , 0));	
	
	return det * retMat;
}


float Deg2Rad(float angle)
{
	return angle * (CV_PI / 180);
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

cv::Point2f Translation(const cv::Mat& SRMatInv, const cv::Point2f& offset, const cv::Point2f& position)
{
	cv::Point2f difference = position - offset;
	cv::Mat difMat(difference);
	cv::Mat resultMat = SRMatInv * difMat;
	
	return cv::Point2f(resultMat);
}


