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

<<<<<<< HEAD
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
=======
	for(int i = 0; i < imageScaled.rows; i++)
	{
		for(int j = 0; j < imageScaled.cols; j++)
		{
			int ii = cvRound(i / scale);
			int jj = cvRound(j / scale);

			imageScaled.at<unsigned char>(i , j) = imageOrginal.at<unsigned char>(ii , jj);
>>>>>>> f75570e0805117639e1a673b7e8f03ff17947a3b
		}
	}

	cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Source image", imageOrginal );

	cv::namedWindow("Translated image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Translated image", imageResult );

	cv::waitKey(0);

<<<<<<< HEAD
=======










	//cv::Mat C = (cv::Mat_<float>(2, 2) << 10.0f, 11.0f, 4.0f, 6.0f);
	//cv::Mat D = C.inv();
	////// proof result = identy matrix for both E and F
	////cv::Mat E = C * D;
	////cv::Mat F = D * C;
	//cv::Mat inverseMatrix = Inverse(C);




	//cv::Mat SRMat = ScaleRotationMatrix(1.0f, 0.0f);
	//cv::Mat inverseSRMat = SRMat.inv();

	//


	//std::string imageFile = "Data/face1.jpg";

	//cv::Mat imageOrginal = cv::imread(imageFile, CV_LOAD_IMAGE_GRAYSCALE);
	//if(!imageOrginal.data)
	//{
	//	std::cout << "Could not open or find the image"  << std::endl;		
	//	std::exit(EXIT_FAILURE);
 //   }

	//
	//cv::Mat imageScaled = cv::Mat::zeros(imageOrginal.rows, imageOrginal.cols, imageOrginal.type()); 

	//

	//cv::Point center = cv::Point(imageOrginal.cols / 2, imageOrginal.rows / 2);
	//double angle = 50.0;
	//double scale = 2.0;

	//
	//cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, scale);
	//
	//cv::warpAffine(imageOrginal, imageScaled, rot_mat, imageOrginal.size());

	//cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE );
	//cv::imshow("Source image", imageOrginal );

	//cv::namedWindow("Warp", CV_WINDOW_AUTOSIZE );
	//cv::imshow("Warp", imageScaled );


	//cv::waitKey(0);


>>>>>>> f75570e0805117639e1a673b7e8f03ff17947a3b
	return 0;
}

float Determinant(const cv::Mat& inMat)
{	
	return (inMat.at<float>(0 , 0) * inMat.at<float>(1 , 1)) - (inMat.at<float>(0 , 1) * inMat.at<float>(1 , 0));	
}

cv::Mat Inverse(const cv::Mat& inMat)
{
<<<<<<< HEAD
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

=======
	float det = 1.0f / Determinant(inMat);
	
>>>>>>> f75570e0805117639e1a673b7e8f03ff17947a3b
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

<<<<<<< HEAD
	return (cv::Mat_<float>(2, 2) <<  cosTheta, -sinTheta,
		                              sinTheta,  cosTheta);
=======
	return (cv::Mat_<float>(2, 2) << scale * cosTheta, scale * -sinTheta,
		                         scale * sinTheta, scale *  cosTheta);	
>>>>>>> f75570e0805117639e1a673b7e8f03ff17947a3b
}

cv::Point2f Translation(const cv::Mat& SRMatInv, const cv::Point2f& offset, const cv::Point2f& position)
{
	cv::Point2f difference = position - offset;
	cv::Mat difMat(difference);
	cv::Mat resultMat = SRMatInv * difMat;
	
	return cv::Point2f(resultMat);
}


