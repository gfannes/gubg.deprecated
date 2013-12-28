#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace cv;

int main(int, char**)
{
    VideoCapture cap(1);
    if(!cap.isOpened()) return -1;

    Mat frame, edges;
    namedWindow("edges",1);
    for(;;)
    {
        cap >> frame;
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        imshow("frame", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}


