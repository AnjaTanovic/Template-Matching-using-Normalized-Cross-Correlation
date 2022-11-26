#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include<sstream>
#include<fstream>
#include<iomanip>
#include <cstdlib>
#include <time.h>

using namespace std;
using namespace cv;

Mat img_orig, tmpl_orig;
Mat img, tmpl;
Mat bgr_img[3];
Mat bgr_tmpl[3];  
int picture_num = 1;
int tr = 0;
int space = 0;
int rand_rows;
int rand_channel_img, rand_channel_tmpl;
int x = 0;    //prevention from writting template values multiple times

void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations);
void cross_Correlation(char *img_arr, char *tmpl_arr, const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols);

int main(int argc, char *argv[])
{
	srand(time(NULL));
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


		/*for(int i = 0; i < 10; i++)
		{
		//Execution of template matching with randomization channels and rows of img
			templateMatching(bgr_img, bgr_tmpl, tr, locations);
			picture_num++;
		} */ 
			 
		templateMatching(bgr_img, bgr_tmpl, tr, locations);
		picture_num++;
			
			   // x = 0;
			}
	return EXIT_SUCCESS;
}

void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations)
{
	int p = 0;
	int img_rows = bgr_img[0].rows;
    int img_cols = bgr_img[0].cols;
    int tmpl_rows = bgr_tmpl[0].rows;
    int tmpl_cols = bgr_tmpl[0].cols;
    
    //1D arrays for matrices
    char *img_arr = new char[img_rows*img_cols];
    char *tmpl_arr = new char[tmpl_rows*tmpl_cols];
    
    //STORING PIXELS IN TXT FILES
    
    ofstream myfile_img, myfile_tmpl;
    
    rand_channel_img = rand();
    while(rand_channel_img > 3)
    {
    	rand_channel_img = rand();
    }
    
     rand_channel_tmpl = rand();
    while(rand_channel_tmpl > 3)
    {
    	rand_channel_tmpl = rand();
    }
    
    rand_rows = rand();
    while(rand_rows > img_rows - tmpl_rows)
    {
		rand_rows = rand();
    }
    
    
 	/*string rand_channel_s = to_string(rand_channel); 
 	char const *rand_channel_c = rand_channel_s.c_str();
 	string rand_rows_s = to_string(rand_rows); 
 	char const *rand_rows_c = rand_rows_s.c_str();*/
 	
	char image_name[35]= "img_file_";
	
	string num_str = to_string(picture_num);
	char const *tmp = num_str.c_str();	
	strcat(image_name, tmp);
	
	strcat(image_name,".h");
	/*strcat(image_name, "_");
	strcat(image_name, rand_channel_c);
	strcat(image_name, "_");
	strcat(image_name, rand_rows_c);
	strcat(image_name,".h");*/
			
    myfile_img.open (image_name);
    
    myfile_img << "int image_stripe[] =" << endl;
    myfile_img << "{" << endl;
    
    //arrays are for parameters in CrossCorellation function
    for (int j = rand_rows; j < rand_rows + tmpl_rows; j++)
	{
		for (int k = 0; k < img_cols; k++)
		{
			myfile_img << (int)bgr_img[rand_channel_img].at<char>(j ,k);
			img_arr[p * img_cols + k] = bgr_img[rand_channel_img].at<char>(j ,k);
			if(k == img_cols - 1  && p ==  tmpl_rows -1)
			{
				myfile_img << endl;
			}
			else
			{
				myfile_img << ",";
			}
			if (space == 20)
			{
				myfile_img << endl;
				space = 0;
			}
			space ++;
			
		}
		p++;
	}
    p = 0;
    space = 0;
	myfile_img << "};" << endl;	
    myfile_img.close();
      
	for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < tmpl_cols; k++)
			{
				tmpl_arr[j * tmpl_cols + k] = bgr_tmpl[rand_channel_tmpl].at<char>(j ,k);
			}
		}
		
//  if(x == 0)
//	{
		//Rand Channel , template
		char tmpl_name[35]= "tmpl_file_";	
		strcat(tmpl_name,tmp);
		//strcat(tmpl_name,"_0");
		strcat(tmpl_name,".h");
				
		myfile_tmpl.open (tmpl_name);
		
		myfile_tmpl << "int tmpl_stripe[] =" << endl;
    	myfile_tmpl << "{" << endl;
		
		for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < tmpl_cols; k++)
			{
				myfile_tmpl << (int)bgr_tmpl[rand_channel_tmpl].at<char>(j ,k);
				if(k == tmpl_cols - 1  && j == tmpl_rows - 1)
				{
					myfile_tmpl << endl;
				}
				else
				{
					myfile_tmpl << ",";
				}
				if (space == 20)
			{
				myfile_tmpl << endl;
				space = 0;
			}
			space ++;
			}
		}
		myfile_tmpl << "};" << endl;
		myfile_tmpl.close();
		
		space = 0;
		//Dimensions 
		/*char dimensions_name[35]= "dimensions_file_";
		
		strcat(dimensions_name,tmp);
		strcat(dimensions_name,".h");
		
		myfile_tmpl.open (dimensions_name);
		myfile_tmpl << (int)bgr_tmpl[0].cols << endl;
		myfile_tmpl << (int)bgr_tmpl[0].rows << endl;
		//myfile_tmpl << (int)bgr_img[0].rows << endl;
		myfile_tmpl << (int)bgr_img[0].cols << endl;
		
		
		
		myfile_tmpl.close();*/
//	}
	//	x = 1;	
		cross_Correlation(img_arr, tmpl_arr, img_rows, img_cols, tmpl_rows, tmpl_cols);
		delete[] img_arr;
		delete[] tmpl_arr;
}


void cross_Correlation(char *img_arr, char *tmpl_arr, const int img_rows,const int img_cols,const int tmpl_rows,const int tmpl_cols)
{
    long int resp, sum1, sum2;
    int sqrt_o;
    ofstream myfile_resp, myfile_sum1, myfile_sum2;
    
   /* string rand_channel_s = to_string(rand_channel); 
			 	char const *rand_channel_c = rand_channel_s.c_str();
			 	string rand_rows_s = to_string(rand_rows); 
			 	char const *rand_rows_c = rand_rows_s.c_str();*/
			 	
				char resp_name[35]= "resp_file_";
				char sum1_name[35] = "sum1_file_";
				char sum2_name[35] = "sum2_file_";
						
				string num_str = to_string(picture_num);
				char const *tmp = num_str.c_str();
				
				strcat(resp_name, tmp);
				strcat(resp_name,".h");
				
				strcat(sum1_name, tmp);
				strcat(sum1_name,".h");
			
				strcat(sum2_name, tmp);
				strcat(sum2_name,".h");
				
			/*	strcat(resp_name, "_");
				strcat(resp_name, rand_channel_c);
				strcat(resp_name, "_");
				strcat(resp_name, rand_rows_c);
				strcat(resp_name,".h");
				
				strcat(sum1_name, tmp);
				strcat(sum1_name, "_");
				strcat(sum1_name, rand_channel_c);
				strcat(sum1_name, "_");
				strcat(sum1_name, rand_rows_c);
				strcat(sum1_name,".h");
				
				strcat(sum2_name, tmp);
				strcat(sum2_name, "_");
				strcat(sum2_name, rand_channel_c);
				strcat(sum2_name, "_");
				strcat(sum2_name, rand_rows_c);
				strcat(sum2_name,".h");*/
						
				myfile_resp.open (resp_name);
				myfile_sum1.open (sum1_name);
				myfile_sum2.open (sum2_name);
				
				myfile_resp << "int resp[] =" << endl;
				myfile_sum1 << "int sum1[] =" << endl;
				myfile_sum2 << "int sum2[] =" << endl;
				
				myfile_resp << "{" << endl;
				myfile_sum1 << "{" << endl;
				myfile_sum2 << "{" << endl;
				
    
    for (int j = 0; j < img_cols - tmpl_cols; j++)
            {
                resp = 0;
                sum1 = 0;
                sum2 = 0;
                for (int k = 0; k < tmpl_rows; k++)
                {
                    for (int m = 0; m < tmpl_cols; m++)
                    {
			   			 resp += img_arr[j + k * img_cols + m] * tmpl_arr[k * tmpl_cols + m];
			   			 sum1 += img_arr[j + k * img_cols + m] * img_arr[ j +  k * img_cols + m];
			   			 sum2 += tmpl_arr[k * tmpl_cols + m] * tmpl_arr[k * tmpl_cols + m];
			   			
                    }
                }   
                
                     
				 myfile_resp << fixed  << resp;
			   	 myfile_sum1 << fixed << sum1 ;
			 	 myfile_sum2 << fixed << sum2 ;
			 	 if( j != img_cols - tmpl_cols - 1)
				{
					myfile_resp << ",";
					myfile_sum1 << ",";
					myfile_sum2 << ",";
				} 
				if (space == 5)
				{
					myfile_resp << endl;
					myfile_sum1 << endl;
					myfile_sum2 << endl;
					space = 0;
				}
				space ++;
			 	 
            }
            
	myfile_resp << "};" << endl;
	myfile_sum1 << "};" << endl;
	myfile_sum2 << "};" << endl;
	space = 0;  
    myfile_resp.close();
    myfile_sum1.close();
    myfile_sum2.close();
}


