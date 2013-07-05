#include <opencv2\highgui\highgui.hpp>
#include <iostream>


int main()
{
	cv::Mat image = cv::imread("Data/Face1.JPG", CV_LOAD_IMAGE_COLOR);
	if(!image.data)
	{
		std::cout << "Could not open or find the image"  << std::endl;
		//system("pause"); //wait for a key press
		return -1;
    }

	
	cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
	cv::moveWindow( "Display window", 300, 100);
	cv::imshow( "Display window", image); //display the image which is stored in the 'img' in the "MyWindow" window


	cv::waitKey(0); //wait infinite time for a keypress

	cv::destroyWindow("Display window"); //destroy the window with the name, "MyWindow"

	return 0;
}
