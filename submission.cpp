#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

Mat image, mask, bgdModel, fgdModel, result, resultGrey;
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

    rect = Rect(Point(30, 100), Point(width-30, height-30));

    mask.create(image.size(), CV_8UC1);

    grabCut( image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT );

    mask = mask & 1;

    image.copyTo(result, mask);

    cvtColor( result, resultGrey, COLOR_BGR2GRAY );
    blur( resultGrey, resultGrey, Size(5,5) );

    int thresh = 200;
    Mat canny_output;
    Canny( resultGrey, canny_output, thresh, thresh*2 );

    vector<vector<Point> > contours;
     vector<vector<Point> > contours0;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours0, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

    Mat drawing = Mat::zeros( result.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( 255, 0, 0 );
        drawContours( drawing, contours0, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }
    
    
    imshow("result", drawing);
    waitKey(0);

    return 0;

}