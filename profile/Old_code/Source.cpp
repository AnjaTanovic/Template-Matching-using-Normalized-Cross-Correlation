#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "templatematching.hpp"

using namespace std;
using namespace cv;

Mat img; Mat templ; Mat result;
int picture_num =0;
int match_method;

void MatchingMethod();

int main(int argc, char *argv[])
{
	for (int j = 0; j < 10; j++ )
	{
		for (int i = 1; i < argc; i = i + 2)
    		{		
        		img = imread(argv[i], IMREAD_COLOR);
        		templ = imread(argv[i+1], IMREAD_COLOR);
        		MatchingMethod();
    		}
	}	
    return EXIT_SUCCESS;
}

void MatchingMethod()
{
    Mat img_display;
    img.copyTo(img_display);

    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    match_method = TM_CCOEFF_NORMED;
    matchTemplate1(img, templ, result, match_method);

    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    Mat threshold = result >= 0.85;

    Mat locations;   // output, locations of non-zero pixels on threshold, matrix with 1 col, elements are [x,y]
    findNonZero(threshold, locations);

    int match_number = locations.rows; //number of matched pixels

    int i = 0;
    while (i < match_number)
    {
        rectangle(img_display, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + templ.cols, locations.at<Point>(i).y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);
      	//color red, thickness 2, line type 8, shift
        rectangle(result, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + templ.cols, locations.at<Point>(i).y + templ.rows), Scalar(0, 0, 255), 2, 8, 0); 
       	//color red, thickness 2, line type 8, shift
        i++;
    }

    char picture_name[35]= "Res/";
    string pom = to_string(picture_num);
    char const *tmp = pom.c_str();

    strcat(picture_name,tmp);
    strcat(picture_name,".jpg");
    
    imwrite(picture_name,img_display);
    picture_num++;

    return;
}
