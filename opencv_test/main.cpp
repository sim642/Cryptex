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

    pair<int, int> hval(0, 179);
    pair<int, int> sval(0, 255);
    pair<int, int> vval(0, 255);

    createTrackbar("hmin", "Video 1", &hval.first, 179);
    createTrackbar("hmax", "Video 1", &hval.second, 179);
    createTrackbar("smin", "Video 1", &sval.first, 255);
    createTrackbar("smax", "Video 1", &sval.second, 255);
    createTrackbar("vmin", "Video 1", &vval.first, 255);
    createTrackbar("vmax", "Video 1", &vval.second, 255);

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
        auto lower = Scalar(hval.first, sval.first, vval.first);
        auto upper = Scalar(hval.second, sval.second, vval.second);

        Mat mask;
        inRange(hsv, lower, upper, mask);

        Mat filtered;
        frame.copyTo(filtered, mask);
        imshow("Video 1", filtered);

        auto key = waitKey(30);
        switch (key)
        {
            case 27: // ESC
            case 'q':
                return EXIT_SUCCESS;
            case 'h':
                mirror_h ^= 1;
                break;
            case 'v':
                mirror_v ^= 1;
                break;
        }

        auto pixel = frame.at<Vec3b>(0, 0);
        cout << int(pixel[0]) << "\t" << int(pixel[1]) << "\t" << int(pixel[2]) << endl;
    }

    return EXIT_SUCCESS;
}
