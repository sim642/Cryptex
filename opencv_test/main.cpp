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

    bool mirror_h = false, mirror_v = false;

    Scalar_<int> lower(0, 0, 0), upper(179, 255, 255);

    {
        FileStorage fs("calib.yml", FileStorage::READ);
        fs["lower"] >> lower;
        fs["upper"] >> upper;
    }

    createTrackbar("hmin", "Video 1", &lower[0], 179);
    createTrackbar("hmax", "Video 1", &upper[0], 179);
    createTrackbar("smin", "Video 1", &lower[1], 255);
    createTrackbar("smax", "Video 1", &upper[1], 255);
    createTrackbar("vmin", "Video 1", &lower[2], 255);
    createTrackbar("vmax", "Video 1", &upper[2], 255);

    while (1)
    {
        Mat frame;
        cap >> frame;

        if (mirror_h)
            flip(frame, frame, 1);
        if (mirror_v)
            flip(frame, frame, 0);

        Mat hsv;
        cvtColor(frame, hsv, CV_BGR2HSV);

        Mat mask;
        inRange(hsv, Scalar(lower), Scalar(upper), mask);

        Mat filtered;
        frame.copyTo(filtered, mask);
        imshow("Video 1", filtered);

        auto key = waitKey(30);
        switch (key)
        {
            case 27: // ESC
            case 'q':
                goto quit;
            case 'h':
                mirror_h ^= 1;
                break;
            case 'v':
                mirror_v ^= 1;
                break;
        }
    }

quit:

    {
        FileStorage fs("calib.yml", FileStorage::WRITE);
        fs << "lower" << lower;
        fs << "upper" << upper;
    }

    return EXIT_SUCCESS;
}
