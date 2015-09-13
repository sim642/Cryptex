#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>

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

    Scalar_<int> lower(0, 0, 0), upper(179, 255, 255), maxs(179, 255, 255);

    {
        FileStorage fs("calib.yml", FileStorage::READ);
        fs["lower"] >> lower;
        fs["upper"] >> upper;
    }

    string chans = "HSV";
    for (int i = 0; i < chans.size(); i++)
    {
        createTrackbar(string(1, chans[i]) + string("min"), "Video 1", &lower[i], maxs[i]);
        createTrackbar(string(1, chans[i]) + string("max"), "Video 1", &upper[i], maxs[i]);
    }

    namedWindow("Video 2");

    SimpleBlobDetector::Params params;
    params.filterByColor = true;
    params.blobColor = 255;
    params.filterByArea = false;
    params.filterByCircularity = false;
    params.filterByConvexity = false;
    params.filterByInertia = true;
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

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
        auto structuring = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
        dilate(mask, mask, structuring);
        erode(mask, mask, structuring);

        imshow("Video 2", mask);

        Mat filtered;
        frame.copyTo(filtered, mask);

        vector<KeyPoint> keypoints;
        detector->detect(mask, keypoints);

        auto largest = max_element(keypoints.begin(), keypoints.end(), [](const KeyPoint &lhs, const KeyPoint &rhs)
        {
            return lhs.size < rhs.size;
        });

        Mat keyImg;
        filtered.copyTo(keyImg);

        if (largest != keypoints.end())
            circle(keyImg, largest->pt, largest->size / 2, Scalar(255, 0, 255), 5);

        //drawKeypoints(filtered, keypoints, keyImg, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        imshow("Video 1", keyImg);

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
