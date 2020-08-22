#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

Mat image, mask, bgdModel, fgdModel, result;
int width, height;
Rect rect;

int
main()
{
    string filename = "./scanned-form.jpg";
    image = imread(filename, IMREAD_COLOR);
    width = image.size().width;
    height = image.size().height;

    namedWindow("result", WINDOW_NORMAL);
    resizeWindow("result", width/2, height/2);

    rect = Rect(Point(20, 100), Point(width-20, height-20));

    mask.create(image.size(), CV_8UC1);

    grabCut( image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT );

    mask = mask & 1;

    image.copyTo(result, mask);

    imshow("result", result);
    waitKey(0);

    return 0;

}