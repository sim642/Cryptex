#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap(1);

    if (!cap.isOpened())
    {
		cerr << "FAIL: capture open" << endl;
		return EXIT_FAILURE;
    }

    namedWindow("Video 1");

    Mat frame;
    cap >> frame;
    imshow("Video 1", frame);

    waitKey(0);

    return EXIT_SUCCESS;
}
