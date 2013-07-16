#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <ctime>

float Determinant(const float* const ptr);
cv::Mat Inverse(const cv::Mat& inMat);

cv::Mat ScaleMatrix(float scale);
cv::Mat RotationMatrix(float angle);
float Deg2Rad(float angle);
cv::Mat CreateInverseScaleRotateMatrix(float scale, float rotZ);
void ImTrans(const cv::Mat& src, cv::Mat& dest, const cv::Mat& invA, const cv::Mat& t);
void ImTrans1(const cv::Mat& src, cv::Mat& dest, const cv::Mat& A, const cv::Mat& t);


int main()
{
	cv::Mat grayImage = cv::imread("Data/face5.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if(!grayImage.data)
	{
		std::cout << "Could not open or find the image"  << std::endl;		
		std::exit(EXIT_FAILURE);
	}

	


	

	float tX = 0.0f;
	float tY = 0.0f;	
	cv::Mat offset = (cv::Mat_<float>(2, 1) <<  tX, tY);

	float scale = 1.0f;
	float rotZ = 0.0f;
	cv::Mat SRMatInv = CreateInverseScaleRotateMatrix(scale, rotZ);


	cv::Mat sc = RotationMatrix(rotZ);
	sc = scale * sc;

	clock_t start_time = clock();
	cv::Mat imageResult = cv::Mat::zeros(grayImage.rows, grayImage.cols, grayImage.type());
	ImTrans(grayImage, imageResult, SRMatInv, offset);
	clock_t finis_time = clock();
	double result = (finis_time - start_time);
	std::cout << result << std::endl;

	cv::namedWindow("Translated image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Translated image", imageResult);


	clock_t start_time1 = clock();	
	cv::Mat imResult = cv::Mat::zeros(grayImage.rows, grayImage.cols, grayImage.type());
	ImTrans1(grayImage, imResult, sc, offset);	
	clock_t finis_time1 = clock();
	double result1 = (finis_time1 - start_time1);
	std::cout << result1;

	cv::namedWindow("Translated1 image", CV_WINDOW_AUTOSIZE );
	cv::imshow("Translated1 image", imResult);

	cv::waitKey(0);
	
	return 0;
}


inline float Determinant(const float* const ptr)
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


inline float Deg2Rad(float angle)
{
	return angle * (float)(CV_PI / 180.0f);
}

inline cv::Mat ScaleMatrix(float s)
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

cv::Mat CreateInverseScaleRotateMatrix(float scale, float rotZ)
{	
	return Inverse(ScaleMatrix(scale) * RotationMatrix(rotZ));
}

cv::Mat CreateScaleRotateMatrix(float scale, float rotZ)
{	
	return ScaleMatrix(scale) * RotationMatrix(rotZ);
}



void ImTrans(const cv::Mat& src, cv::Mat& dest, const cv::Mat& invA, const cv::Mat& t)
{
	cv::Rect srcRect(0, 0, src.cols, src.rows);
	
	const uchar* const srcPtr = src.data;

	for(int y = 0; y < dest.rows; ++y)
	{
		uchar* const destPtr = dest.ptr<uchar>(y);		

		for(int x = 0; x < dest.cols; ++x)
		{
			cv::Mat resultMat = invA * ( (cv::Mat_<float>(2, 1) <<  x, y) - t );

			cv::Point2i result(resultMat);			

			if(!srcRect.contains(result))
			{
				continue;
			}
			
			destPtr[x] = srcPtr[src.step * result.y + result.x];						
		}
	}
}

void ImTrans1(const cv::Mat& src, cv::Mat& dest, const cv::Mat& A, const cv::Mat& t)
{
	cv::Mat_<float> m(2, 3);

	A.col(0).copyTo(m.col(0));
	A.col(1).copyTo(m.col(1));
	t.col(0).copyTo(m.col(2));

	cv::warpAffine(src, dest, m, dest.size(), cv::INTER_NEAREST);
}
