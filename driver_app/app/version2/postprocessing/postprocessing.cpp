#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

Mat img_orig, tmpl_orig;
Mat img, tmpl;

int tr;

int toInt( char *buf);

int main(int argc, char *argv[])
{
	tr = toInt(argv[1]);
	
	img_orig = imread(argv[2], IMREAD_COLOR);
	
	tmpl_orig = imread(argv[3], IMREAD_COLOR);
	
	img_orig.convertTo(img, CV_8SC3);
	normalize(img, img, -128, 127, NORM_MINMAX);
	
	tmpl_orig.convertTo(tmpl, CV_8SC3);
	normalize(tmpl, tmpl, -128, 127, NORM_MINMAX);
	
	int img_rows = img.rows;
	int img_cols = img.cols;
	int tmpl_rows = tmpl.rows;
	int tmpl_cols = tmpl.cols;

	Mat locations;   // Locations of non-zero pixels on threshold, matrix with 1 col, elements are [x,y]
	
	double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)];
	
	fstream myfile;
	myfile.open(argv[4]);
	
	int k = 0;
	double num;
	while (myfile >> num) 
	{
		res[k] = num;
		k++;
	}
	myfile.close();
	
	Mat treshold;
		treshold = Mat::ones(img_rows - tmpl_rows, img_cols - tmpl_cols, CV_8UC1);

		for (int i = 0; i < img_rows - tmpl_rows; i++)
		{
		for (int j = 0; j < img_cols - tmpl_cols; j++)
		{
				if (res[i*(img_cols-tmpl_cols)+ j] <= tr)
				treshold.at<uchar>(i, j) = 0;   // zero points are points where treshold is NOT satisfied
		}
		}
	
	delete[] res;
	
		findNonZero(treshold, locations); // treshold is satisfied at non zero points
		
		int match_number = locations.rows; //number of matched pixels
	int i = 0;
	
	while (i < match_number)
	{
		rectangle(img_orig, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + tmpl.cols, locations.at<Point>(i).y + tmpl.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift
		i++;
	}
	
	char picture_name[35]= "result_picture.jpg";
		
	imwrite(picture_name,img_orig);
	
	return 0;
}
	
int toInt( char *buf)
{
	int val = 0;
	val += (((int)buf[0]) - '0') * 100;
	val += (((int)buf[1]) - '0') * 10;
	val += (((int)buf[2]) - '0') * 1;

	return val;
}
