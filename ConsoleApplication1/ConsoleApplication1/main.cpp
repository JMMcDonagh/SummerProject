#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>


struct Vec2i
{
	inline Vec2i() {}
	inline Vec2i(int _x, int _y) : x(_x), y(_y) {}

	int x, y;
};

struct Vec2f
{
	inline Vec2f() {}
	inline Vec2f(float _x, float _y) : x(_x), y(_y) {}

	float x, y;
};

struct Index
{
	inline Index() {}
	inline Index(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}

	int a, b, c;
};


void LoadShape(const std::string& fileName, Vec2i* out);
void LoadShapef(const std::string& fileName, Vec2f* out);
void LoadIndices(const std::string& fileName, Index* out);
void LoadTextureBase(const std::string& fileName, const Vec2i& resolution, std::vector<Vec2i> trianglePointsList[]);

void WarpImage(const Vec2i* const meanPoints, const Vec2i* const currentPoints, const Index* const indices, int triCount, const std::vector<Vec2i>* const alist, const cv::Mat& inputImage, cv::Mat& outImage);
void WarpImagef(const Vec2f* const meanPoints, const Vec2f* const currentPoints, const Index* const indices, int triCount, const std::vector<Vec2i>* const trianglePointsList, const cv::Mat& inputImage, cv::Mat& outImage);

int main()
{
	const int triangleCount = 111;
	const int trianglePoints = 68;
	const Vec2i resolution(169, 167);

	std::string meanShape = "Data/Mean_Shape.txt";
	std::string currentShape = "Data/Current_Shape.txt";
	std::string triangles = "Data/Triangles.txt";
	std::string textureBase = "Data/Texture_Base.txt";
	std::string imageName = "Data/inputImage.png";

	cv::Mat inputImage = cv::imread(imageName, CV_LOAD_IMAGE_GRAYSCALE);
	if(!inputImage.data)
	{
		std::cerr << "Could not open or find the image"  << std::endl;		
		std::exit(EXIT_FAILURE);
	}	

	std::vector<Vec2i> trianglePointsList[triangleCount];
	LoadTextureBase(textureBase, resolution, trianglePointsList);

	Vec2i* meanPoints = new Vec2i[trianglePoints];	
	LoadShape(meanShape, meanPoints);

	Vec2i* currentPoints = new Vec2i[trianglePoints];	
	LoadShape(currentShape, currentPoints);




	Vec2f* meanPointsf = new Vec2f[trianglePoints];	
	LoadShapef(meanShape, meanPointsf);

	Vec2f* currentPointsf = new Vec2f[trianglePoints];	
	LoadShapef(currentShape, currentPointsf);




	Index* indices = new Index[triangleCount];
	LoadIndices(triangles, indices);


	cv::Mat outImage = cv::Mat::zeros(resolution.y, resolution.x, inputImage.type());

	clock_t timeC;
	int loops = 100;
	timeC = clock();	
	for(int y = 0; y < loops; ++y)
	{
		WarpImage(meanPoints, currentPoints, indices, triangleCount, trianglePointsList, inputImage, outImage);
	}	
	timeC = clock() - timeC;
	std::cout << "WarpImage time to do " << loops << " loops: " << timeC * 1000 / CLOCKS_PER_SEC << " milliseconds." << std::endl;

	cv::Mat outImagef = cv::Mat::zeros(resolution.y, resolution.x, inputImage.type());
	timeC = clock();	
	for(int y = 0; y < loops; ++y)
	{
		WarpImagef(meanPointsf, currentPointsf, indices, triangleCount, trianglePointsList, inputImage, outImagef);
	}	
	timeC = clock() - timeC;
	std::cout << "WarpImagef time to do " << loops << " loops: " << timeC * 1000 / CLOCKS_PER_SEC << " milliseconds." << std::endl;


	cv::namedWindow("WarpImage image", CV_WINDOW_AUTOSIZE );
	cv::imshow("WarpImage image", outImage);

	cv::namedWindow("WarpImage imagef", CV_WINDOW_AUTOSIZE );
	cv::imshow("WarpImage imagef", outImagef);

	cv::waitKey(0);

	return 0;
}

void LoadShape(const std::string& fileName, Vec2i* out)
{
	std::ifstream fin(fileName);
	if(!fin.is_open())
	{
		std::cerr << "Error file " << fileName << " not found.";
		std::exit(EXIT_FAILURE);
	}

	float a, b;
	int i = 0;
	while (fin >> a >> b)
	{
		out[i++] = Vec2i((int)a, (int)b);		
	}
	fin.close();
}

void LoadIndices(const std::string& fileName, Index* out)
{
	std::ifstream fin(fileName);
	if(!fin.is_open())
	{
		std::cerr << "Error file " << fileName << " not found.";
		std::exit(EXIT_FAILURE);
	}

	int a, b, c;
	int i = 0;
	while (fin >> a >> b >> c)
	{
		out[i++] = Index(--a, --b, --c);		
	}
	fin.close();
}

void LoadTextureBase(const std::string& fileName, const Vec2i& resolution, std::vector<Vec2i> trianglePointsList[])
{
	std::ifstream fin(fileName);
	if(!fin.is_open())
	{
		std::cerr << "Error file " << fileName << " not found.";
		std::exit(EXIT_FAILURE);
	}

	int width = resolution.x;
	int height = resolution.y;
	int temp;


	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			fin >> temp;
			if(temp == 0)
			{
				continue;
			}
			trianglePointsList[--temp].push_back(Vec2i(x, y));
		}
	}	
}

void WarpImage(const Vec2i* const meanPoints,
	           const Vec2i* const currentPoints,
			   const Index* const indices,
			   int triCount,
			   const std::vector<Vec2i>* const trianglePointsList,
			   const cv::Mat& inputImage,
			   cv::Mat& outImage)
{
	const uchar* const srcPtr = inputImage.data;
	uchar* destPtr = outImage.data;	

	Vec2i src[3];
	Vec2i dest[3];
	Vec2i currentPos;
	float X, Y;	
	float transMatrix[6];
	float inverseMatrix[9];

	int triangleCount = triCount;
	int inStep = inputImage.step;
	int outStep = outImage.step;
	

	for(int currentTriangle = 0; currentTriangle < triangleCount; currentTriangle++)
	{
		Index currentIndex = indices[currentTriangle];		

		src[0] = currentPoints[currentIndex.a];
		src[1] = currentPoints[currentIndex.b];
		src[2] = currentPoints[currentIndex.c];

		dest[0] = meanPoints[currentIndex.a];
		dest[1] = meanPoints[currentIndex.b];
		dest[2] = meanPoints[currentIndex.c];

		int determinant = (dest[0].x * (dest[1].y - dest[2].y)) - (dest[1].x * (dest[0].y - dest[2].y)) + (dest[2].x * (dest[0].y - dest[1].y));
		float invDet = 1.0f / determinant;

		inverseMatrix[0] =  (dest[1].y - dest[2].y) * invDet;
		inverseMatrix[1] = -(dest[1].x - dest[2].x) * invDet;
		inverseMatrix[2] =  (dest[1].x * dest[2].y - dest[2].x * dest[1].y) * invDet;

		inverseMatrix[3] = -(dest[0].y - dest[2].y) * invDet;
		inverseMatrix[4] =  (dest[0].x - dest[2].x) * invDet;
		inverseMatrix[5] = -(dest[0].x * dest[2].y - dest[0].y * dest[2].x) * invDet;

		inverseMatrix[6] =  (dest[0].y - dest[1].y) * invDet;
		inverseMatrix[7] = -(dest[0].x - dest[1].x) * invDet;
		inverseMatrix[8] =  (dest[0].x * dest[1].y - dest[0].y * dest[1].x) * invDet;


		transMatrix[0] = src[0].x * inverseMatrix[0] + src[1].x * inverseMatrix[3] + src[2].x * inverseMatrix[6];
		transMatrix[1] = src[0].x * inverseMatrix[1] + src[1].x * inverseMatrix[4] + src[2].x * inverseMatrix[7];
		transMatrix[2] = src[0].x * inverseMatrix[2] + src[1].x * inverseMatrix[5] + src[2].x * inverseMatrix[8];
		transMatrix[3] = src[0].y * inverseMatrix[0] + src[1].y * inverseMatrix[3] + src[2].y * inverseMatrix[6];
		transMatrix[4] = src[0].y * inverseMatrix[1] + src[1].y * inverseMatrix[4] + src[2].y * inverseMatrix[7];
		transMatrix[5] = src[0].y * inverseMatrix[2] + src[1].y * inverseMatrix[5] + src[2].y * inverseMatrix[8];


		int count = trianglePointsList[currentTriangle].size();
		for(int index = 0; index < count; index++)
		{
			currentPos = trianglePointsList[currentTriangle][index];
			float tempX = (float)currentPos.x;
			float tempY = (float)currentPos.y;

			X = transMatrix[0] * tempX + transMatrix[1] * tempY + transMatrix[2];
			Y = transMatrix[3] * tempX + transMatrix[4] * tempY + transMatrix[5];					

			destPtr[outStep * currentPos.y + currentPos.x] = srcPtr[inStep * (int)Y + (int)X];
		}
	}
}





void LoadShapef(const std::string& fileName, Vec2f* out)
{
	std::ifstream fin(fileName);
	if(!fin.is_open())
	{
		std::cerr << "Error file " << fileName << " not found.";
		std::exit(EXIT_FAILURE);
	}

	float a, b;
	int i = 0;
	while (fin >> a >> b)
	{
		out[i++] = Vec2f(a, b);		
	}
	fin.close();
}


void WarpImagef(const Vec2f* const meanPoints,
	            const Vec2f* const currentPoints,
				const Index* const indices,
				int triCount,
				const std::vector<Vec2i>* const trianglePointsList,
				const cv::Mat& inputImage,
				cv::Mat& outImage)
{
	const uchar* const srcPtr = inputImage.data;
	uchar* destPtr = outImage.data;	

	Vec2f src[3];
	Vec2f dest[3];
	Vec2i currentPos;
	float X, Y;	
	float transMatrix[6];
	float inverseMatrix[9];

	int triangleCount = triCount;
	int inStep = inputImage.step;
	int outStep = outImage.step;


	for(int currentTriangle = 0; currentTriangle < triangleCount; currentTriangle++)
	{
		Index currentIndex = indices[currentTriangle];		

		src[0] = currentPoints[currentIndex.a];
		src[1] = currentPoints[currentIndex.b];
		src[2] = currentPoints[currentIndex.c];

		dest[0] = meanPoints[currentIndex.a];
		dest[1] = meanPoints[currentIndex.b];
		dest[2] = meanPoints[currentIndex.c];

		float determinant = (dest[0].x * (dest[1].y - dest[2].y)) - (dest[1].x * (dest[0].y - dest[2].y)) + (dest[2].x * (dest[0].y - dest[1].y));
		float invDet = 1.0f / determinant;

		inverseMatrix[0] =  (dest[1].y - dest[2].y) * invDet;
		inverseMatrix[1] = -(dest[1].x - dest[2].x) * invDet;
		inverseMatrix[2] =  (dest[1].x * dest[2].y - dest[2].x * dest[1].y) * invDet;

		inverseMatrix[3] = -(dest[0].y - dest[2].y) * invDet;
		inverseMatrix[4] =  (dest[0].x - dest[2].x) * invDet;
		inverseMatrix[5] = -(dest[0].x * dest[2].y - dest[0].y * dest[2].x) * invDet;

		inverseMatrix[6] =  (dest[0].y - dest[1].y) * invDet;
		inverseMatrix[7] = -(dest[0].x - dest[1].x) * invDet;
		inverseMatrix[8] =  (dest[0].x * dest[1].y - dest[0].y * dest[1].x) * invDet;


		transMatrix[0] = src[0].x * inverseMatrix[0] + src[1].x * inverseMatrix[3] + src[2].x * inverseMatrix[6];
		transMatrix[1] = src[0].x * inverseMatrix[1] + src[1].x * inverseMatrix[4] + src[2].x * inverseMatrix[7];
		transMatrix[2] = src[0].x * inverseMatrix[2] + src[1].x * inverseMatrix[5] + src[2].x * inverseMatrix[8];
		transMatrix[3] = src[0].y * inverseMatrix[0] + src[1].y * inverseMatrix[3] + src[2].y * inverseMatrix[6];
		transMatrix[4] = src[0].y * inverseMatrix[1] + src[1].y * inverseMatrix[4] + src[2].y * inverseMatrix[7];
		transMatrix[5] = src[0].y * inverseMatrix[2] + src[1].y * inverseMatrix[5] + src[2].y * inverseMatrix[8];


		int count = trianglePointsList[currentTriangle].size();
		for(int index = 0; index < count; index++)
		{
			currentPos = trianglePointsList[currentTriangle][index];
			float tempX = (float)currentPos.x;
			float tempY = (float)currentPos.y;

			X = transMatrix[0] * tempX + transMatrix[1] * tempY + transMatrix[2];
			Y = transMatrix[3] * tempX + transMatrix[4] * tempY + transMatrix[5];					

			destPtr[outStep * currentPos.y + currentPos.x] = srcPtr[inStep * (int)Y + (int)X];
		}
	}
}