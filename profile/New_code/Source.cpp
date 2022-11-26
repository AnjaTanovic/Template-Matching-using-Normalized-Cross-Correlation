#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat img_orig, tmpl_orig;
Mat img, tmpl;
Mat bgr_img[3];
Mat bgr_tmpl[3];  
int picture_num = 0;

//Threshold parameter for matching (tr can be number between 0 and 255, more than 200 is recommended)
int tr = 0;


void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[], int tr, Mat& locations);
void cross_Correlation(char *img_arr, char *tmpl_arr,double *res, const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols);
int toInt( char *buf);

int main(int argc, char *argv[])
{

    //Mat img_orig = imread("../../data/pumpkins.jpg", IMREAD_COLOR);
    //Mat tmpl_orig = imread("../../data/pumpkins_template.jpg", IMREAD_COLOR);

    tr = toInt(argv[1]);
    for (int h = 2; h < argc; h = h + 2)
    {		
        img_orig = imread(argv[h], IMREAD_COLOR);
        tmpl_orig = imread(argv[h+1], IMREAD_COLOR);
        
        string imgWindow = "Original image";
        string tmplWindow = "Template image";
        /*cv::namedWindow(imgWindow, WINDOW_AUTOSIZE);
        string tmplWindow = "Template image";
        cv::namedWindow(tmplWindow, WINDOW_AUTOSIZE);

        cv::imshow(imgWindow, img_orig);
        cv::imshow(tmplWindow, tmpl_orig);
        cv::waitKey(0);
        */
        //Conversion for Cross Correlation (the results are better if images have both negative and positive pixel values) 
        
        img_orig.convertTo(img, CV_8SC3);
        normalize(img, img, -128, 127, NORM_MINMAX);
        tmpl_orig.convertTo(tmpl, CV_8SC3);
        normalize(tmpl, tmpl, -128, 127, NORM_MINMAX);

        // Spliting images to blue, green, red (blue - 0, green - 1, red - 2)
        
        split(img, bgr_img); 
         
        split(tmpl, bgr_tmpl);

        /*
        imwrite("blue.png", bgr_img[0]); //blue channel
        imwrite("green.png", bgr_img[1]); //green channel
        imwrite("red.png", bgr_img[2]); //red channel

        imwrite("blue1.png", bgr_tmpl[0]); //blue channel
        imwrite("green1.png", bgr_tmpl[1]); //green channel
        imwrite("red1.png", bgr_tmpl[2]); //red channel
        */

        int img_rows = img.rows;
        int img_cols = img.cols;
        int tmpl_rows = tmpl.rows;
        int tmpl_cols = tmpl.cols;
    


        Mat locations;   // Locations of non-zero pixels on threshold, matrix with 1 col, elements are [x,y]

        //Execution of template matching
        templateMatching(bgr_img, bgr_tmpl, tr, locations);

        //Drawing rectangles around found templates

        int match_number = locations.rows; //number of matched pixels
        int i = 0;
        while (i < match_number)
        {
            rectangle(img_orig, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + tmpl.cols, locations.at<Point>(i).y + tmpl.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift
            i++;
        }

        /*cv::imshow(imgWindow, img_orig);
        cv::waitKey(0);
        
        cv::destroyAllWindows();*/
        char picture_name[35]= "picture_";
        string pom = to_string(picture_num);
        char const *tmp = pom.c_str();

        strcat(picture_name,tmp);
        strcat(picture_name,".jpg");
            
        imwrite(picture_name,img_orig);
            
        picture_num++;

    }
 
    return EXIT_SUCCESS;
}

void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[], int tr, Mat & locations)
{
    int img_rows = bgr_img[0].rows;
    int img_cols = bgr_img[0].cols;
    int tmpl_rows = bgr_tmpl[0].rows;
    int tmpl_cols = bgr_tmpl[0].cols;

    /* Mat transformed to 2D arrays
     char img_arr[3][img_rows][img_cols];
     char tmpl_arr[3][tmpl_rows][tmpl_cols];
    */

    //1D arrays for matrices
    char *img_arr = new char[3*img_rows*img_cols];
    char *tmpl_arr = new char[3*tmpl_rows*tmpl_cols];

    for (int i = 0; i < 3; i++)
    {
	for (int j = 0; j < img_rows; j++)
	{
		for (int k = 0; k < img_cols; k++)
		{
			img_arr[i*(img_rows*img_cols)+j* img_cols+k] = bgr_img[i].at<char>(j,k);
		}
	}
    }
 
    for (int i = 0; i < 3; i++)
    {
	for (int j = 0; j < tmpl_rows; j++)
	{
		for (int k = 0; k < tmpl_cols; k++)
		{
			tmpl_arr[i*(tmpl_rows*tmpl_cols)+j* tmpl_cols+k] = bgr_tmpl[i].at<char>(j,k);
		}
	}
    }

    //Final matrix which includes all 3 channels
    
    double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)];

    for (int i = 0; i < img_rows - tmpl_rows; i++)
    {
	    for(int j = 0; j < img_cols - tmpl_cols; j++)
	    {
		    res[i*(img_cols-tmpl_cols) + j] = 0;
	    }
    }

    cross_Correlation((char *)img_arr,(char *)tmpl_arr,(double *)res,img_rows,img_cols,tmpl_rows,tmpl_cols);

    delete[] img_arr;
    delete[] tmpl_arr;

    //  Mat res_ch;
    // res_ch = Mat(img_rows - tmpl_rows, img_cols - tmpl_cols, CV_64FC1, &res);
	
  
    //normalize(res, res, 0, 255, NORM_MINMAX);
    
    double max = res[0];
    double min = res[0];

    //Searching for min and max (for normalization
    
    for (int i = 0; i < img_rows - tmpl_rows; i++)
    {
	    for (int j = 0; j < img_cols - tmpl_cols; j++)
	    {
		    if (max < res[i*(img_cols-tmpl_cols) + j])
			    max = res[i*(img_cols-tmpl_cols) + j];	
		    if (min > res[i*(img_cols-tmpl_cols) + j])
			    min = res[i*(img_cols-tmpl_cols) + j];	
	    }
    }

    //Normalization
	
    for (int i = 0; i < img_rows - tmpl_rows; i++)
    {
	    for (int j = 0; j < img_cols - tmpl_cols; j++)
	    {
		    res[i*(img_cols - tmpl_cols) + j]  = ((res[i*(img_cols-tmpl_cols) + j] - min) * 255) / (max - min);
	    }
    }

    //  res_ch.convertTo(res_ch, CV_8UC1);

    //Cross Correlation result
    /*  string resWindow = "Result image";
    cv::namedWindow(resWindow, WINDOW_AUTOSIZE);
    cv::imshow(resWindow, res_ch);
    cv::waitKey(0);
    */
    
    //Creating a new image to help localisation templates (pixels with value 1 are found templates)
    
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

    /*cv::imshow(resWindow, tmp);
    cv::waitKey(0);
    */
    
    //Threshold matrix, inverting tmp image for findNonZero function applied later  
    //Mat threshold = tmp == 255;

    findNonZero(treshold, locations); // treshold is satisfied at non zero points

    delete[] res;
}

void cross_Correlation(char *img_arr, char *tmpl_arr, double *res,const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols)
{
    //Matrices for calculating Cross Correlation
    //Mat res;
    double resp, sum1, sum2;
    int sqrt_o;
    //Mat pom1,pom2;
    
    //Cross Correlation
    for (int ch = 0; ch < 3; ch++)
    {
        for (int i = 0; i < img_rows - tmpl_rows; i++)
        {
            for (int j = 0; j < img_cols - tmpl_cols; j++)
            {
                resp = 0;
                sum1 = 0;
                sum2 = 0;
                //The distance is calculated for each point of original image (i, j) 
                for (int k = 0; k < tmpl_rows; k++)
                {
                    for (int m = 0; m < tmpl_cols; m++)
                    {
			    resp += (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m))) * (*(tmpl_arr + ch*(tmpl_rows)*(tmpl_cols) + k*(tmpl_cols) + m));
			    sum1 += (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m))) *  (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m))); 
			    sum2 += (*(tmpl_arr + ch*(tmpl_cols)*(tmpl_rows) + k*(tmpl_cols) + m)) *  (*(tmpl_arr + ch*(tmpl_rows)*(tmpl_cols) + k*(tmpl_cols) + m));
			    // resp += (bgr_img[ch].at<char>(i + k, j + m) * bgr_tmpl[ch].at<char>(k, m));
			    // pom1 += bgr_img[ch].at<char>(i + k, j + m) * bgr_img[ch].at<char>(i + k, j + m);
			    // pom2 += bgr_tmpl[ch].at<char>(k, m) * bgr_tmpl[ch].at<char>(k, m);
                    }
                }
                sqrt_o = sqrt(sum1 * sum2);
                resp /= sqrt_o;
                *(res + i*(img_cols - tmpl_cols) + j)  += resp;

            }
        }
    }

}

int toInt( char *buf)
{
    int val = 0;
    val += (((int)buf[0]) - '0') * 100;
    val += (((int)buf[1]) - '0') * 10;
	val += (((int)buf[2]) - '0') * 1;

    return val;
    }
