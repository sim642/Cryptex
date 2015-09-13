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
    int thres = 127;

    createTrackbar("thres", "Video 1", &thres, 255);

    while (1)
    {
        Mat frame;
        cap >> frame;

        if (mirror_h)
            flip(frame, frame, 1);
        if (mirror_v)
            flip(frame, frame, 0);

        Mat gray(frame.size(), CV_8UC1);
        cvtColor(frame, gray, CV_BGR2GRAY);
        threshold(gray, gray, thres, 255, THRESH_BINARY);
        imshow("Video 1", gray);

        auto key = waitKey(30);
        switch (key)
        {
            case 27: // ESC
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
