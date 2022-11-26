#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include<sstream>
#include<fstream>
#include<iomanip>

using namespace std;
using namespace cv;

Mat img_orig, tmpl_orig;
Mat img, tmpl;
Mat bgr_img[3];
Mat bgr_tmpl[3];  
int picture_num = 0;
int tr = 0;

void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations);

int main(int argc, char *argv[])
{
	//tr = toInt(argv[1]);
    for (int h = 2; h < argc; h = h + 2)
    {		
        img_orig = imread(argv[h], IMREAD_COLOR);
        tmpl_orig = imread(argv[h+1], IMREAD_COLOR);
        
        //Conversion for Cross Correlation (the results are better if images have both negative and positive pixel values) 
        
        img_orig.convertTo(img, CV_8SC3);
        normalize(img, img, -128, 127, NORM_MINMAX);
        tmpl_orig.convertTo(tmpl, CV_8SC3);
        normalize(tmpl, tmpl, -128, 127, NORM_MINMAX);

        // Spliting images to blue, green, red (blue - 0, green - 1, red - 2)
        
        split(img, bgr_img); 
         
        split(tmpl, bgr_tmpl);


        int img_rows = img.rows;
        int img_cols = img.cols;
        int tmpl_rows = tmpl.rows;
        int tmpl_cols = tmpl.cols;
    
        int memory_check;
		
		//template check
		memory_check = (tmpl_cols*tmpl_rows);
		if(memory_check > 51000)
		{
			cout<<"Sorry, you need to check if template dimensions are too large, system doesn't have enough BRAM memory." << endl;
			exit(0);
		}
	
		//image check
		memory_check = (tmpl_rows*img_cols);
		if(memory_check > 160000)
		{
			cout<<"Sorry, you need to check if image dimensions are too large, system doesn't have enough BRAM memory." << endl;
			exit(0);
		}
		
		//result check
		memory_check = (img_cols-tmpl_cols);
		if(memory_check > 800)
		{
			cout<<"Sorry, you need to check if template or image dimensions are too large, system doesn't have enough BRAM memory." << endl;
			exit(0);
		}

		if(tmpl_cols > 255 || tmpl_rows > 255 || img_cols >1023)
		{
			cout<<"Sorry, you need to check if template or image dimensions are too large." << endl;
			exit(0);
		}

        Mat locations;   // Locations of non-zero pixels on threshold, matrix with 1 col, elements are [x,y]

        //Execution of template matching
        templateMatching(bgr_img, bgr_tmpl, tr, locations);
            
        picture_num++;

    }
	return EXIT_SUCCESS;
}

void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations)
{
	int img_rows = bgr_img[0].rows;
    int img_cols = bgr_img[0].cols;
    int tmpl_rows = bgr_tmpl[0].rows;
    int tmpl_cols = bgr_tmpl[0].cols;
    
    //1D arrays for matrices
    char *img_arr = new char[3*img_rows*img_cols];
    char *tmpl_arr = new char[3*tmpl_rows*tmpl_cols];
    
    //STORING PIXELS IN TXT FILES
    
    ofstream myfile_img, myfile_tmpl;
    
    //Channel 0, image
	char image_name[35]= "image_";
	string num_str = to_string(picture_num);
	char const *tmp = num_str.c_str();
	
	strcat(image_name,tmp);
	strcat(image_name,"_0");
	strcat(image_name,".txt");
			
    myfile_img.open (image_name);
    
    for (int j = 0; j < img_rows; j++)
	{
		for (int k = 0; k < img_cols; k++)
		{
			myfile_img << (int)bgr_img[0].at<char>(j ,k) << endl;
		}
	}
    
    myfile_img.close();
    
    //Channel 1, image
    
    for (int r = 0; r < 50; r++)
    {
    	if (image_name[r] == '.')
    	{
    		image_name[r-1] = '1';
    		break;
    	}
    }
			
    myfile_img.open (image_name);
    
    for (int j = 0; j < img_rows; j++)
	{
		for (int k = 0; k < img_cols; k++)
		{
			myfile_img << (int)bgr_img[1].at<char>(j ,k) << endl;
		}
	}
	
	myfile_img.close();
	
	//Channel 2, image
    
    for (int r = 0; r < 50; r++)
    {
    	if (image_name[r] == '.')
    	{
    		image_name[r-1] = '2';
    		break;
    	}
    }
			
    myfile_img.open (image_name);
    
    for (int j = 0; j < img_rows; j++)
	{
		for (int k = 0; k < img_cols; k++)
		{
			myfile_img << (int)bgr_img[2].at<char>(j ,k) << endl;
		}
	}
	
	myfile_img.close();
	
	//Channel 0, template
	char tmpl_name[35]= "template_";
	
	strcat(tmpl_name,tmp);
	strcat(tmpl_name,"_0");
	strcat(tmpl_name,".txt");
			
    myfile_tmpl.open (tmpl_name);
    
    for (int j = 0; j < tmpl_rows; j++)
	{
		for (int k = 0; k < tmpl_cols; k++)
		{
			myfile_tmpl << (int)bgr_tmpl[0].at<char>(j ,k) << endl;
		}
	}
    
    myfile_tmpl.close();
    
    //Channel 1, template
    
    for (int r = 0; r < 50; r++)
    {
    	if (tmpl_name[r] == '.')
    	{
    		tmpl_name[r-1] = '1';
    		break;
    	}
    }
			
    myfile_tmpl.open (tmpl_name);
    
    for (int j = 0; j < tmpl_rows; j++)
	{
		for (int k = 0; k < tmpl_cols; k++)
		{
			myfile_tmpl << (int)bgr_tmpl[1].at<char>(j ,k) << endl;
		}
	}
	
	myfile_tmpl.close();
	
	//Channel 2, template
    
    for (int r = 0; r < 50; r++)
    {
    	if (tmpl_name[r] == '.')
    	{
    		tmpl_name[r-1] = '2';
    		break;
    	}
    }
			
    myfile_tmpl.open (tmpl_name);
    
    for (int j = 0; j < tmpl_rows; j++)
	{
		for (int k = 0; k < tmpl_cols; k++)
		{
			myfile_tmpl << (int)bgr_tmpl[2].at<char>(j ,k) << endl;
		}
	}
	
	myfile_tmpl.close();
	
	//Dimensions 
	char dimensions_name[35]= "dimensions_";
	
	strcat(dimensions_name,tmp);
	strcat(dimensions_name,".txt");
	
	myfile_tmpl.open (dimensions_name);
    
	myfile_tmpl << (int)bgr_img[0].rows << endl;
	myfile_tmpl << (int)bgr_img[0].cols << endl;
	myfile_tmpl << (int)bgr_tmpl[0].rows << endl;
	myfile_tmpl << (int)bgr_tmpl[0].cols << endl;
	
	myfile_tmpl.close();
	
}
