#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#define SC_INCLUDE_FX
#include <systemc>
#include <iostream>
#include <deque>
#include <vector>
#include <cmath>
#include <typeinfo>

using namespace std;
using namespace cv;

const int W1=32;
typedef sc_dt::sc_int <W1> typa;
typedef sc_dt::sc_fix typb;
int W2=20;
int F2=3;
typedef sc_dt::sc_fix typc;
int W3=32;
int F3=32;
typedef vector<typb> type_b;

Mat img_orig, tmpl_orig;
Mat img, tmpl;
Mat bgr_img[3];
Mat bgr_tmpl[3];  
int picture_num = 0;

template <typename R, typename P, typename S>
void cross_Correlation(char *img_arr, char *tmpl_arr, R &res, const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols);

int sc_main(int argc, char *argv[])
{

    for (int h = 1; h < argc; h = h + 2)
    {		
        img_orig = imread(argv[h], IMREAD_COLOR);
        tmpl_orig = imread(argv[h+1], IMREAD_COLOR);

        string imgWindow = "Original image";
        string tmplWindow = "Template image";

        
        img_orig.convertTo(img, CV_8SC3);
        normalize(img, img, -128, 127, NORM_MINMAX);
        tmpl_orig.convertTo(tmpl, CV_8SC3);
        normalize(tmpl, tmpl, -128, 127, NORM_MINMAX);


        
        split(img, bgr_img);        
        split(tmpl, bgr_tmpl);

	vector<typb> x(W2,F2);

        int img_rows = img.rows;
        int img_cols = img.cols;
        int tmpl_rows = tmpl.rows;
        int tmpl_cols = tmpl.cols;

        int tr = 240;
        Mat locations; 
         
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

	    //cross Correlation using type double for matrices
	 //   cross_Correlation((char *)img_arr,(char *)tmpl_arr,(double *)res,img_rows,img_cols,tmpl_rows,tmpl_cols);	   
	    
	    
	    cross_Correlation<double*, double,int> ((char *)img_arr,(char *)tmpl_arr,res,img_rows,img_cols,tmpl_rows,tmpl_cols);

	    double mis;
	    
	    int flag1 = 1;
            int flag2 = 0;
	   
	   
	   while(flag1)
	   { 
		//cross Correlation using sc_int and sc_fix types for matrices
	   	
	   	type_b res_b;
		    for (int i = 0; i < img_rows - tmpl_rows; i++)
		    {
			    for(int j = 0; j < img_cols - tmpl_cols; j++)
			    {
			    		typb t(W2, F2);
			    		t=0;
					res_b.push_back(t);
			    }
		    }
		    
		cross_Correlation<type_b, typa, typc> ((char *)img_arr,(char *)tmpl_arr,res_b,img_rows,img_cols,tmpl_rows,tmpl_cols);
	   	
	   	for(int i = 0; i < img_rows - tmpl_rows; i++) 
	    	{
	    		
		    for(int j = 0; j < img_cols - tmpl_cols; j++)
		    {
		    	
			mis = abs((res[i*(img_cols-tmpl_cols) + j]  - res_b.at(i*(img_cols-tmpl_cols) + j) ) / res[i*(img_cols-tmpl_cols) + j]) ;
			/*cout <<"Miss is: "<< mis << endl;
			cout << res[i*(img_cols-tmpl_cols) + j] << endl;
			cout << res_b.at(i*(img_cols-tmpl_cols) + j) << endl;
			cout<< endl;*/
			if(mis > 0.01)
			{
				flag2 = 1;
				W2++;
				//cout << W2 << endl;
				break;	
			}
		    }
		    if(flag2)
	   	    {
	    			break;
	    	    }
	    	}
	    		   	
	   	if(!flag2)
	   	{
	   		flag1 = 0; //Miss is below 0.1%
	   	}
	   	flag2=0;
	   }
	
        cout<< "Photo: " << argv[h] << ". Width of output matrix is " << W2 << ". Positon of point is "<< F2 << ",  after point is "<< W2 - F2 << " bits. " << endl;
        
        
    delete[] img_arr;
    delete[] tmpl_arr;
	
  
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
    
	findNonZero(treshold, locations); // treshold is satisfied at non zero points

    
        //Execution of template matching

        //Drawing rectangles around found templates

        int match_number = locations.rows; //number of matched pixels
        int i = 0;
        while (i < match_number)
        {
            rectangle(img_orig, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + tmpl.cols, locations.at<Point>(i).y + tmpl.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift
            i++;
        }

        
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



template <typename R, typename P, typename S>
void cross_Correlation(char *img_arr, char *tmpl_arr, R &res,const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols)
{
    
    // double resp, pom1, pom2;
    //double resp_p;
    //int sqrt_o;
    
    P resp; 
    P pom1;
    P pom2;

    S sqrt_o;
 
    //Cross Correlation
    for (int ch = 0; ch < 3; ch++)
    {
        for (int i = 0; i < img_rows - tmpl_rows; i++)
        {
            for (int j = 0; j < img_cols - tmpl_cols; j++)
            {
                resp = 0;
                pom1 = 0;
                pom2 = 0;
                sqrt_o = 0;
                
                //The distance is calculated for each point of original image (i, j) 
                for (int k = 0; k < tmpl_rows; k++)
                {
                    for (int m = 0; m < tmpl_cols; m++)
                    {
			resp += (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m))) * (*(tmpl_arr + ch*(tmpl_rows)*(tmpl_cols) + k*(tmpl_cols) + m));
		      	pom1 +=  (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m))) *  (*(img_arr + ch*(img_cols)*(img_rows) + ((i+k)*(img_cols) + j+m)));  
			pom2 +=	 (*(tmpl_arr + ch*(tmpl_cols)*(tmpl_rows) + k*(tmpl_cols) + m)) *  (*(tmpl_arr + ch*(tmpl_rows)*(tmpl_cols) + k*(tmpl_cols) + m));

                       
                    }
                }
                sqrt_o = sqrt(pom1 * pom2);
                res [i*(img_cols - tmpl_cols) + j]  += resp / sqrt_o;
		
            }
        }
    }
}
