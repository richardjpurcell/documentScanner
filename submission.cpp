//docs.opencv.org/master/db/d00/samples_2cpp_2squares_8cpp-example.html#a20

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include <iostream>

using namespace std;
using namespace cv;

Mat image, mask, bgdModel, fgdModel, result, resultGrey;
int width, height;
Rect rect;

static double angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

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

    int thresh = 20;
    Mat canny_output;
    Canny( resultGrey, canny_output, thresh, thresh*2 );

    vector<vector<Point> > contours;
    vector<Point> approx;
    vector<vector<Point> > cornerPoints;

    findContours( canny_output, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


    for(size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);

        if( approx.size() == 4 &&
            fabs(contourArea(approx)) > 1000 &&
            isContourConvex(approx))
        {
            double maxCosine = 0;
            for(int j = 2; j < 5; j++)
            {
                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            if(maxCosine < 0.3)
                cornerPoints.push_back(approx);
        }

    }

    //order points by y value
    for(int i=0; i < cornerPoints[0].size()-1; i++)
    {
        Point temp = cornerPoints[0][i];
        if(cornerPoints[0][i].y > cornerPoints[0][i+1].y)
        {
            cornerPoints[0][i] = cornerPoints[0][i+1];
            cornerPoints[0][i+1] = temp;
        }
    }
    //order top points
    if(cornerPoints[0][0].x > cornerPoints[0][1].x)
    {
        Point temp = cornerPoints[0][0];
        cornerPoints[0][0] = cornerPoints[0][1];
        cornerPoints[0][1] = temp;
    }
    //order bottom points
        if(cornerPoints[0][3].x < cornerPoints[0][4].x)
    {
        Point temp = cornerPoints[0][3];
        cornerPoints[0][3] = cornerPoints[0][4];
        cornerPoints[0][3] = temp;
    }
    //create correctly sized document template given 500px width
    Mat M(500, 647, CV_8U, Scalar(126, 0, 255));
    vector<Point> D = { Point(0,0), Point(0, 500), Point(500, 647), Point(500, 0)};

    Mat h = findHomography( cornerPoints[0], D, RANSAC);

    warpPerspective(image, M, h, M.size());



    Mat drawing = Mat::zeros( result.size(), CV_8UC3 );
    for( size_t i = 0; i < cornerPoints[0].size(); i++ )
    {
        Scalar color = Scalar( 255, 0, 255 );
        polylines(image, cornerPoints[0], true, color);
    }
    
    imshow("result", image);
    waitKey(0);

    return 0;

}