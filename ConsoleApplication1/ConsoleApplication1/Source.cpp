#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <ctime>
#include <emmintrin.h>

float Determinant(const float* const ptr);
cv::Mat Inverse(const cv::Mat& inMat);

cv::Mat ScaleMatrix(float scale);
cv::Mat RotationMatrix(float angle);
float Deg2Rad(float angle);
cv::Mat CreateInverseScaleRotateMatrix(float scale, float rotZ);


void ImTransCV(const cv::Mat& src, cv::Mat& dest, const cv::Mat& mat);
void ImTransSSE(const cv::Mat& src, cv::Mat& dest, const cv::Mat& invA, const cv::Mat& t);


cv::Mat ScaleMatrixH(float s);
cv::Mat RotationMatrixH(float angle);
cv::Mat TranslationMatrixH(float x, float y);

void DrawLine(cv::Mat img, cv::Point start, cv::Point end);

int main()
{
	cv::Mat grayImage = cv::imread("Data/face2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if(!grayImage.data)
	{
		std::cout << "Could not open or find the image"  << std::endl;		
		std::exit(EXIT_FAILURE);
	}

	
	int tX = 0;
	int tY = 0;	
	cv::Mat offset = (cv::Mat_<int>(2, 1) <<  tX, tY);

	float scale = 1.0f;
	float rotZ = 0.0f;
	

	cv::Mat SRMatInv = CreateInverseScaleRotateMatrix(scale, rotZ);

	clock_t timeC;	
	int loops = 100;

	cv::Point2f srcTri[] = { cv::Point2f(0, 0), cv::Point2f(grayImage.cols - 1, 0), cv::Point2f(0, grayImage.rows - 1) };
	cv::Point2f destTri[3];

	cv::Mat tMat = TranslationMatrixH(tX, tY);
	cv::Mat rMat = RotationMatrixH(rotZ);
	cv::Mat sMat = ScaleMatrixH(scale);

	cv::Mat srtMat = tMat * rMat * sMat;

	for(int i = 0; i < 3; i++)
	{
		cv::Point3f pp(srcTri[i].x, srcTri[i].y, 1.0f);
		cv::Mat fff(pp);
		cv::Mat oo = srtMat * fff;
		cv::Point3f ee(oo);
		destTri[i].x = ee.x;
		destTri[i].y = ee.y;
	}

	
	cv::Mat destCV = cv::Mat::zeros(grayImage.rows, grayImage.cols, grayImage.type());	
	cv::Mat affineTransform = cv::getAffineTransform(srcTri, destTri);
	timeC = clock();
	for(int y = 0; y < loops; ++y)
	{
		ImTransCV(grayImage, destCV, affineTransform);	
	}	
	timeC = clock() - timeC;
	std::cout << "ImTransCV time to do " << loops << " loops: " << (float)timeC / CLOCKS_PER_SEC << " seconds." << std::endl;

	

	cv::Mat destSSE = cv::Mat::zeros(grayImage.rows, grayImage.cols, grayImage.type());
	timeC = clock();
	for(int y = 0; y < loops; ++y)
	{
		ImTransSSE(grayImage, destSSE, SRMatInv, offset);
	}
	timeC = clock() - timeC;
	std::cout << "ImTransSSE time to do " << loops << " loops: " << (float)timeC / CLOCKS_PER_SEC << " seconds." << std::endl;


	DrawLine(destCV, destTri[0], destTri[1]);
	DrawLine(destCV, destTri[0], destTri[2]);
	DrawLine(destCV, destTri[1], destTri[2]);

	DrawLine(destCV, srcTri[0], srcTri[1]);
	DrawLine(destCV, srcTri[0], srcTri[2]);
	DrawLine(destCV, srcTri[1], srcTri[2]);

	
	
	cv::namedWindow("ImTransCV image", CV_WINDOW_AUTOSIZE );
	cv::imshow("ImTransCV image", destCV);
	

	cv::namedWindow("ImTransSSE image", CV_WINDOW_AUTOSIZE );
	cv::imshow("ImTransSSE image", destSSE);


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


void ImTransCV(const cv::Mat& src, cv::Mat& dest, const cv::Mat& mat)
{
	cv::warpAffine(src, dest, mat, dest.size(), cv::INTER_NEAREST);
}

void ImTransSSE(const cv::Mat& src, cv::Mat& dest, const cv::Mat& invA, const cv::Mat& t)
{
	const float* const intAPtr = (float*)invA.data;
	const int* const tPtr = (int*)t.data;		

	const uchar* const srcPtr = src.data;
	uchar* destPtr;	
	
	unsigned int rows = dest.rows;
	unsigned int cols = dest.cols;
	unsigned int step = src.step;	

	
	__m128 a = _mm_set_ps1(intAPtr[0]);
	__m128 b = _mm_set_ps1(intAPtr[1]);
	__m128 c = _mm_set_ps1(intAPtr[2]);
	__m128 d = _mm_set_ps1(intAPtr[3]);
	__m128 tX = _mm_set_ps1(tPtr[0]);
	__m128 tY = _mm_set_ps1(tPtr[1]);

	__m128 yyyy, tyy, aaa, bbb, xxxx, txx, X, Y;
	__m128i Xx, Yy;

	for(int y = 0; y < rows; ++y)
	{
		destPtr = dest.ptr<uchar>(y);

		yyyy = _mm_set_ps1(y);
		
		tyy = _mm_sub_ps(yyyy, tY);
		aaa = _mm_mul_ps(b, tyy);
		bbb = _mm_mul_ps(d, tyy);	

		int x;
		for(x = 0; x < cols; x += 4)
		{
			xxxx = _mm_set_ps(x+3, x+2, x+1, x);
			txx = _mm_sub_ps(xxxx, tX);
			
			X = _mm_add_ps(_mm_mul_ps(a, txx), aaa);
			Y = _mm_add_ps(_mm_mul_ps(c, txx), bbb);
			Xx = _mm_cvttps_epi32(X);
			Yy = _mm_cvttps_epi32(Y);
			
			for(unsigned int i = 0; i < 4; i++)
			{
				if((Xx.m128i_u32[i] >= 0 && Xx.m128i_u32[i] < cols) && (Yy.m128i_u32[i] >= 0 && Yy.m128i_u32[i] < rows))
				{
					destPtr[x+i] = srcPtr[step * Yy.m128i_u32[i] + Xx.m128i_u32[i]];
				}
			}
		}		
	}
}


cv::Mat ScaleMatrixH(float s)
{
	return (cv::Mat_<float>(3, 3) << s, 0, 0,
									 0, s, 0,
									 0, 0, 1);
}

cv::Mat RotationMatrixH(float angle)
{
	float theta = Deg2Rad(angle);
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);

	return (cv::Mat_<float>(3, 3) <<  cosTheta, -sinTheta, 0,
									  sinTheta,  cosTheta, 0,
									  0,		 0,		   1);
}

cv::Mat TranslationMatrixH(float x, float y)
{
	return (cv::Mat_<float>(3, 3) << 1, 0, x,
									 0, 1, y,
									 0, 0, 1);

}


void DrawLine(cv::Mat img, cv::Point start, cv::Point end)
{
	int thickness = 2;
	int lineType = 8;

	line(img,
		 start,
		 end,
		 cv::Scalar(255),
		 thickness,
		 lineType);
}
