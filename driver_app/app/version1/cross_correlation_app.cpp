#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include <string.h>
#include <math.h>
#include <time.h>

using namespace std;
using namespace cv;

Mat img_orig, tmpl_orig;
Mat img, tmpl;
Mat bgr_img[3];
Mat bgr_tmpl[3];  
int picture_num = 0;

//Threshold parameter for matching (tr can be number between 0 and 255, more than 200 is recommended)
int tr = 0;

int *stripe_resp;
int *stripe_sum1;
int stripe_sum2;

int tmpl_cols, tmpl_rows, img_cols, img_rows;
int ready;

void templateMatching(int tr, Mat &locations);
void crossCorrelation(double *res);
void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init);
void write_bram(const int *data, int img_tmpl);
void read_ip();
void read_bram();
int toInt(char *buf);

int main ()
{
	char *option;
	char *tresh;
	char *img_name;
	char *tmpl_name;
	
	char *img_loc = "../../../../data/";
	char *tmpl_loc = "../../../../data/";
	
	printf("Choose the number of picture:\n");
	printf("1.bubbles\n");
	printf("2.pumpkins\n");
	printf("3.green\n");
	printf("4.diamonds\n");
	printf("5.ball\n");
	printf("6.logo\n");
	printf("7.stars\n");
	printf("8.sun\n");
	printf("9.baby_yoda\n");
	printf("10.chocolate_bubbles\n");
	printf("11.Choose your own images\n");
    printf("If you want to upload your own images, choose option 11.\n");

	size_t num_b = 1;
	option = (char*) malloc(num_b+1);
	getline(&option, &num_b, stdin);
	
	switch (*option)
	{
		case '1':
			img_orig = imread("../../../../data/bubbles.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/bubbles_template.jpg", IMREAD_COLOR);
			break;
		case '2':
			img_orig = imread("../../../../data/pumpkins.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/pumpkins_template.jpg", IMREAD_COLOR);
			break;
		case '3':
			img_orig = imread("../../../../data/green.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/green_template.jpg", IMREAD_COLOR);
			break;
		case '4':
			img_orig = imread("../../../../data/diamonds.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/diamonds_template.jpg", IMREAD_COLOR);
			break;
		case '5':
			img_orig = imread("../../../../data/ball.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/ball_template.jpg", IMREAD_COLOR);
			break;
		case '6':
			img_orig = imread("../../../../data/logo.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/logo_template.jpg", IMREAD_COLOR);
			break;
		case '7':
			img_orig = imread("../../../../data/stars.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/stars_template.jpg", IMREAD_COLOR);
			break;
		case '8':
			img_orig = imread("../../../../data/sun.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/sun_template.jpg", IMREAD_COLOR);
			break;
		case '9':
			img_orig = imread("../../../../data/baby_yoda.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/yoda_template.jpg", IMREAD_COLOR);
			break;
		case '10':
			img_orig = imread("../../../../data/chocolate_bubbles.jpg", IMREAD_COLOR);
			tmpl_orig = imread("../../../../data/chocolate_bubbles_template.jpg", IMREAD_COLOR);
			break;
		case '11':
			printf("Before entering file names, make sure the images are stored in the data directory.\n\n");
			
			printf("Enter file name for image:\n");
			num_b = 30;
			img_name = (char*) malloc(num_b+1);
			getline(&img_name, &num_b, stdin);
			
			printf("Enter file name for template:\n");
			num_b = 30;
			tmpl_name = (char*) malloc(num_b+1);
			getline(&tmpl_name, &num_b, stdin);
			
			strcat(img_loc, img_name);
			strcat(tmpl_loc, tmpl_name);
			
			img_orig = imread(img_loc, IMREAD_COLOR);
			tmpl_orig = imread(tmpl_loc, IMREAD_COLOR);
			break;
		default:
			printf("The choosen option does not exist\n");
			break;
	}
	
	printf("Choose the value of treshold (recommended 230 or more):\n");
	
	num_b = 3;
	tresh = (char*) malloc(num_b+1);
	getline(&tresh, &num_b, stdin);
	
	tr = toInt(tresh);
	
	//Start of time measurement
	time_t start, end;	
	double dif;
	time (&start);
	
	//Start preprocessing
	
	img_orig.convertTo(img, CV_8SC3);
	normalize(img, img, -128, 127, NORM_MINMAX);
	tmpl_orig.convertTo(tmpl, CV_8SC3);
	normalize(tmpl, tmpl, -128, 127, NORM_MINMAX);

	// Spliting images to blue, green, red (blue - 0, green - 1, red - 2)
	split(img, bgr_img); 
	split(tmpl, bgr_tmpl);

	img_rows = img.rows;
	img_cols = img.cols;
	tmpl_rows = tmpl.rows;
	tmpl_cols = tmpl.cols;
	 
	//Check if system has enough memory 
	
	int memory_check;
		
	//template check
	memory_check = (tmpl_cols * tmpl_rows);
	if(memory_check > 51000)
	{
		cout<<"Sorry, you need to check if template dimensions are too large, system doesn't have enough BRAM memory." << endl;
		exit(0);
	}

	//image check
	memory_check = (tmpl_rows * img_cols);
	if(memory_check > 160000)
	{
		cout<<"Sorry, you need to check if image dimensions are too large, system doesn't have enough BRAM memory." << endl;
		exit(0);
	}
	
	//result check
	memory_check = (img_cols - tmpl_cols);
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

	templateMatching(tr, locations);
	
	//Drawing rectangles around found templates

	int match_number = locations.rows; //number of matched pixels
	int i = 0;
	while (i < match_number)
	{
		rectangle(img_orig, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + tmpl.cols, locations.at<Point>(i).y + tmpl.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift
		i++;
	}

	//End of measurement
	time (&end);
	dif = difftime(end, start);
	printf("The application took %.2lf seconds to run\n", dif);
				
	char picture_name[35]= "result.jpg";
	imwrite(picture_name,img_orig);

	return EXIT_SUCCESS;
}

void templateMatching(int tr, Mat &locations)
{
	
    double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)];

    crossCorrelation((double *)res);
	
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

    delete[] res;
}

void crossCorrelation(double *res)
{
	int stripe = 0;
    int ch = 0; 
    
    double sum = 0;
    int sqrt_o = 0;
	
	stripe_resp = new int[img_cols - tmpl_cols];
	stripe_sum1 = new int[img_cols - tmpl_cols];
	
	//flags
    bool done = 0;
    bool new_ch = 1;
    ready = 1;
    bool need_start = 0; //if this flag is 1 it means that we need to send start bit to ip_hard 
    bool calc_stripe = 0; //for sqrt and division
    
    //Initialize registers for dimensions (flag_init = 1)
    write_ip(tmpl_cols, tmpl_rows, img_cols, 1); 
    
	cout << "Initialisation done by software" << endl;
	
    while (!done)
    {
    
		if(new_ch)
		{
			//tmpl_b defines template in bram.
			int *tmpl_b = new int[tmpl_rows * tmpl_cols];
			
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < tmpl_cols; k++)
				{
					tmpl_b[j * tmpl_cols + k] = bgr_tmpl[ch].at<char>(j,k);
				}
			}

			//Write template in bram (one color)
			write_bram(tmpl_b, 0);

			delete[] tmpl_b;
			
			new_ch = 0;
		}
			
		if (ready)
		{
			// stripe_b is used to change Mat to char*, defines stripe of image in bram
			int *stripe_b = new int[tmpl_rows * img_cols];
		   				
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < img_cols; k++)
				{
					stripe_b[j * img_cols + k]= bgr_img[ch].at<char>(j + stripe,k);
				}
			}
			
			//Write stripe of image in bram 
			write_bram(stripe_b, 1);
			
			delete[] stripe_b;
			
			need_start = 1;
	   	}

		//Ip_hard starts cross correlation (start will reset ready register in ip_hard)
		if (need_start)
		{
			ready = 0;
		    //Start ip and wait until ready becomes 0 (flag_init = 0)
    		write_ip(tmpl_cols, tmpl_rows, img_cols, 0); 
			need_start = 0;
		}

		if (calc_stripe)
		{
			
			for (int j = 0; j < img_cols - tmpl_cols; j++)
			{
				sum = (double)stripe_sum1[j] * (double)stripe_sum2;
				sqrt_o = (int)sqrt(sum);
				
				if ((stripe - 1) >= 0)
					res[(stripe - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;	
				else
					res[(img_rows - tmpl_rows - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;
				
			}
		}

		if (ch == 3)
		{
			done = 1; //exit loop
		}
		
		//Checking if ip_hard needs new stripe (ready) and read stripe_sum2
		read_ip();

		if(ready)
		{	
			
			read_bram();
			
			calc_stripe = 1;  //flag for sqrt and div calculation for next while iteration

			stripe++;
			
			cout << "STRIPE NUMBER :  " << stripe << endl;
			
			if (stripe >= img_rows - tmpl_rows)
			{
				stripe = 0; 
				ch++;
				new_ch = 1;
				
				if (ch == 3)
				{
					//all if conditions have to be disabled (except calc_stripe and ch == 3)
					new_ch = 0;
					ready = 0;
					need_start = 0;
				}
			} 
				
		}	
	}
	
	cout<<"Whole cross correlation done" << endl;
}

void write_ip (const int tmpl_cols, const int tmpl_rows, const int img_cols, const int flag_init)
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "w");
	fprintf (cross_correlation, "%d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
	printf ("[APP] %d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);
	fclose (cross_correlation);
}

void write_bram(const int *data, int img_tmpl)
{
	FILE *bram;
	
	//img_tmpl, 1 for image 0 for template
	
	//Write template
	if (img_tmpl == 0)
	{
		for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < tmpl_cols; k++)
			{
				bram = fopen ("/dev/xlnx,bram_tmpl", "w");
		    	fprintf(bram, "%d, %d\n", j * tmpl_cols + k, data[j * tmpl_cols + k]);
		    	fclose (bram);
			}
		}
	}
	//Write image stripe
	else if (img_tmpl == 1)
	{
		for (int j = 0; j < tmpl_rows; j++)
		{
			for (int k = 0; k < img_cols; k++)
			{
				bram = fopen ("/dev/xlnx,bram_img", "w");
		    	fprintf(bram, "%d, %d\n", j * img_cols + k, data[j * img_cols + k]);
		    	fclose (bram);
			}
		}
	}
	else
	{
		printf("Wrong number for bram\n");
	}

}

void read_ip()
{
	FILE *cross_correlation;
	cross_correlation = fopen ("/dev/xlnx,cross_correlation", "r");
	fscanf (cross_correlation, "ready = %d, sum2 = %d\n", &ready, &stripe_sum2);
	if (ready == 1)
		printf ("[APP] ready = %d, sum2 = %d\n", ready, stripe_sum2);
	fclose (cross_correlation);
}

void read_bram()
{
	FILE *bram;
	int n;
	
	bram = fopen ("/dev/xlnx,bram_resp", "r");		
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d ", &n);
		stripe_resp[j] = n;
   
	}
	fscanf(bram, "\n");
 	fclose (bram);

	bram = fopen ("/dev/xlnx,bram_sum1", "r");		
	for (int j = 0; j < img_cols - tmpl_cols; j++)
	{
		fscanf (bram, "%d", &n);
		stripe_sum1[j] = n;
    	}
	fscanf(bram, "\n");
	fclose (bram);
	
	cout<<("STRIPE DONE !!!!!!!!!! ") << endl;
}

int toInt(char *buf)
{
    int val = 0;
    val += (((int)buf[0]) - '0') * 100;
    val += (((int)buf[1]) - '0') * 10;
	val += (((int)buf[2]) - '0') * 1;

    return val;
}
