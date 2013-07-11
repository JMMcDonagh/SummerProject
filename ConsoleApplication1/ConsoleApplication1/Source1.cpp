#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>


//void xxx(const cv::Mat& in, cv::Mat& out, float scale, float rotation, int trans);

float Determinant(const cv::Mat& inMat);
cv::Mat FindInverse(const cv::Mat& inMat);

float Deg2Rad(float angle);
cv::Mat ScaleRotationMatrix(float scale, float angle);

int main()
{
	float scale = 2.0f;
	std::string imageFile = "Data/face1.jpg";
	cv::Mat imageOrginal = cv::imread(imageFile, CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat imageScaled = cv::Mat::zeros(imageOrginal.rows * scale, imageOrginal.cols * scale, imageOrginal.type());

	for(int i = 0; i < imageScaled.rows; i++)
	{
		for(int j = 0; j < imageScaled.cols; j++)
		{
			int ii = cvRound(i / scale);
			int jj = cvRound(j / scale);

			imageScaled.at<unsigned char>(i , j) = imageOrginal.at<unsigned char>(ii , jj);
		}
	}


	cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Source image", imageOrginal );

	cv::namedWindow("Scaled image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Scaled image", imageScaled );

	cv::waitKey(0);











	//cv::Mat C = (cv::Mat_<float>(2, 2) << 10.0f, 11.0f, 4.0f, 6.0f);
	//cv::Mat D = C.inv();
	////// proof result = identy matrix for both E and F
	////cv::Mat E = C * D;
	////cv::Mat F = D * C;
	//cv::Mat inverseMatrix = FindInverse(C);




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


	return 0;
}

float Determinant(const cv::Mat& inMat)
{	
	return 1.0f / ( (inMat.at<float>(0 , 0) * inMat.at<float>(1 , 1)) - (inMat.at<float>(0 , 1) * inMat.at<float>(1 , 0)) );	
}

cv::Mat FindInverse(const cv::Mat& inMat)
{
	float determinant = Determinant(inMat);
	
	cv::Mat retMat = (cv::Mat_<float>(2, 2) <<  inMat.at<float>(1 , 1), -inMat.at<float>(0 , 1),
											   -inMat.at<float>(1 , 0),  inMat.at<float>(0 , 0));	
	
	return determinant * retMat;
}

//void xxx(const cv::Mat& in, cv::Mat& out, float scale, float angle, float x, float y)
//{
//	float theta = Deg2Rad(angle);
//
//	cv::Mat rot_mat(2, 2, CV_32FC1);
//
//
//
//	float x1 = x * cosf(theta) - y * sinf(theta);
//	float y1 = x * sinf(theta) + y * cosf(theta);
//}

float Deg2Rad(float angle)
{
	return angle * (CV_PI / 180);
}

cv::Mat ScaleRotationMatrix(float scale, float angle)
{
	float theta = Deg2Rad(angle);
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);

	return (cv::Mat_<float>(2, 2) << scale * cosTheta, scale * -sinTheta,
		                             scale * sinTheta, scale *  cosTheta);	
}


//void scale()
//{
//	float scale = 2.0f;
//	std::string imageFile = "Data/face1.jpg";
//	cv::Mat imageOrginal = cv::imread(imageFile, CV_LOAD_IMAGE_GRAYSCALE);
//
//	cv::Mat imageScaled = cv::Mat::zeros(imageOrginal.rows * scale, imageOrginal.cols * scale, imageOrginal.type());
//
//	for(int i = 1; i < imageScaled.rows; i++)
//	{
//		for(int j = 1; j < imageScaled.cols; j++)
//		{
//			int ii = cvRound((i - 1) * (imageOrginal.rows - 1) / (imageScaled.rows - 1) + 1);
//			int jj = cvRound((j - 1) * (imageOrginal.cols - 1) / (imageScaled.cols - 1) + 1);
//
//			imageScaled.at<unsigned char>(i , j) = imageOrginal.at<unsigned char>(ii , jj);
//		}
//	}
//}


