#include "soft.hpp"
#include <chrono>
#include <unistd.h>

using namespace chrono;

int read_ddr_cnt = 0;
int write_ddr_cnt = 0;
int picture_num = 0;
char** data_string;
int argc;
int h ;

SC_HAS_PROCESS(Soft);

Soft::Soft(sc_core::sc_module_name name,char** strings, int argv) : sc_module(name), offset(sc_core::SC_ZERO_TIME)
{
    SC_THREAD(soft);
    SC_REPORT_INFO("Soft", "Constructed.");
	data_string = strings;
	argc = argv;
}
Soft::~Soft()
{
    SC_REPORT_INFO("Soft", "Destroyed.");
}
void Soft::soft()
{ 
		int tr = Treshold_convert(data_string[1]);  
		for (h = 2; h < argc; h+=2)
		{
			cout << "Software started working" << endl;
	
			Mat img_orig, tmpl_orig;
			Mat img, tmpl;
			Mat bgr_img[3];
			Mat bgr_tmpl[3];  

			img_orig = imread(data_string[h], IMREAD_COLOR);
			tmpl_orig = imread(data_string[h+1], IMREAD_COLOR);

			string imgWindow = "Original image";
			string tmplWindow = "Template image";
			
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
		memory_check = (tmpl_cols*tmpl_rows) + (tmpl_rows*img_cols) + 12 * (img_cols-tmpl_cols);

		if(memory_check > 220000)
		{
			cout<<"Sorry, you need to check if template or image dimensions are too large, system doesn't have enough BRAM memory." << endl;
			exit(0);
		}
		if(tmpl_cols > 255 || tmpl_rows > 255 || img_cols >1023)
		{
			cout<<"Sorry, you need to check if template or image dimensions are too large." << endl;
			exit(0);
		}

			//Threshold parameter for matching (tr can be number between 0 and 255, more than 200 is recommended)
			
			
			Mat locations;   // Locations of non-zero pixels on threshold, matrix with 1 col, elements are [x,y]

			templateMatching(bgr_img, bgr_tmpl, tr, locations);

			//Execution of template matching
			int match_number = locations.rows; //number of matched pixels
			int i = 0;
			while (i < match_number)
			{
				rectangle(img_orig, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + tmpl.cols, locations.at<Point>(i).y + tmpl.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift
				i++;
			}
			
			// Printing more pictures. 
			char picture_name[35]= "picture_";
			string pom = to_string(picture_num);
			char const *tmp = pom.c_str();

			strcat(picture_name,tmp);
			strcat(picture_name,".jpg");
			
			imwrite(picture_name,img_orig);

			picture_num++;
		}
}

void Soft::templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations)
{
    int img_rows = bgr_img[0].rows;
    int img_cols = bgr_img[0].cols;
    int tmpl_rows = bgr_tmpl[0].rows;
    int tmpl_cols = bgr_tmpl[0].cols;
    

    // Loading image and template matrix in memory

    //1D arrays for matrices
    char *img_arr = new char[3*img_rows*img_cols];
    char *tmpl_arr = new char[3*tmpl_rows*tmpl_cols];
    
    cout << "Cross correlation is about to start" << endl;
	
    double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)]; //result matrix
    int stripe = 0;
    int ch = 0; 
    
    double sum = 0;
    int sqrt_o = 0;
    
    int *stripe_resp = new int[img_cols - tmpl_cols];
	int *stripe_sum1 = new int[img_cols - tmpl_cols];
	int *stripe_sum2 = new int[img_cols - tmpl_cols];
    
    //flags
    bool done = 0;
    bool new_ch = 1;
    int ready = 1;
    bool need_start = 0; //if this flag is 1 it means that we need to send start bit to ip_hard 
    bool calc_stripe = 0; //for sqrt and division
    
    //Initialize registers for dimensions
    write_hard(ADDR_TMPL_ROWS, tmpl_rows); 
    write_hard(ADDR_TMPL_COLS, tmpl_cols);
    write_hard(ADDR_IMG_COLS, img_cols);
    
	cout << "Initialisation done by software" << endl;
	
    while (!done)
    {
    
		if(new_ch)
		{
			//tmpl_b defines template in bram.
			char *tmpl_b = new char[tmpl_rows * tmpl_cols];
			
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < tmpl_cols; k++)
				{
					tmpl_b[j * tmpl_cols + k] = bgr_tmpl[ch].at<char>(j,k);
				}
			}

			//Write template in bram (one color)
			write_bram(0, tmpl_b, tmpl_rows * tmpl_cols);

			delete[] tmpl_b;
			
			new_ch = 0;
		}
			
		if (ready)
		{
			// stripe_b is used to change Mat to char*, defines stripe of image in bram
			char *stripe_b = new char[tmpl_rows * img_cols];
		   				
			for (int j = 0; j < tmpl_rows; j++)
			{
				for (int k = 0; k < img_cols; k++)
				{
					stripe_b[j * img_cols + k]= bgr_img[ch].at<char>(j + stripe,k);
				}
			}
			
			//Write stripe of image in bram 
			write_bram(tmpl_rows* tmpl_cols, stripe_b, img_cols * tmpl_rows);
			
			delete[] stripe_b;
			
			need_start = 1;
	   	}

		//Ip_hard starts cross correlation (start will reset ready register in ip_hard)
		if (need_start)
		{
			write_hard(ADDR_START,1);
			need_start = 0;
		}
		
		//Waiting for ip_hard to reset ready (need_new_stripe) (when it is 0, exit loop)
		while(ready)
		{
		        ready = read_hard(ADDR_READY);
		        if (!ready)
		            write_hard(ADDR_START,0);
		}

		if (calc_stripe)
		{
			// Checking time needed to do sqrt and div 
			auto start = steady_clock::now();
			
			for (int j = 0; j < img_cols - tmpl_cols; j++)
			{
			
				sum = (double)stripe_sum1[j] * (double)stripe_sum2[j];
				sqrt_o = (int)sqrt(sum);
				
				if ((stripe - 1) >= 0)
					res[(stripe - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;	
				else
					res[(img_rows - tmpl_rows - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;
					
				
			}
			auto end = steady_clock::now();
			
			cout << "Required time for sqrt and div is: " << duration_cast<nanoseconds>(end - start).count() << " ns" << endl;
			
		}

		if (ch == 3)
		{
			done = 1; //exit loop
		}
		
		//Checking if ip_hard needs new stripe
		ready = read_hard(ADDR_READY);

		if(ready)
		{	
			
			read_bram((tmpl_rows*tmpl_cols) + (tmpl_rows*img_cols), stripe_resp, stripe_sum1, stripe_sum2, 3*(img_cols - tmpl_cols));
			
			calc_stripe = 1;  //flag for sqrt and div calculation for next while iteration

			stripe++;
			
			cout << "stripe: " << stripe << endl;
			cout<<"After stripe " << stripe << " simulation time is:" << offset << endl;

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
	
	cout<<"After whole cross correlation, simulation time is:" << offset << endl;
	
	delete[] stripe_resp;
	delete[] stripe_sum1;
	delete[] stripe_sum2;
	
    delete[] img_arr;
    delete[] tmpl_arr;

	// print read and write ddr throughput
	//cout <<"write ddr: "<<write_ddr_cnt << endl;
	//cout <<"read ddr: "<<read_ddr_cnt << endl;
	// Printing data for frame. 
	
	fstream myfile;
	myfile.open("frame_rate.txt",fstream::app);
	myfile <<"---------- Picture ----------" << endl;
	myfile <<"----------"<< data_string[h] <<"----------"<<endl;

	myfile <<"---------- Picture dimension----------" << endl;
	myfile <<"----------"<< img_rows<< "x" << img_cols <<"----------"<<endl;

	myfile <<"---------- Tempalte dimension ----------" << endl;
	myfile <<"----------"<< tmpl_rows << "x"<< tmpl_cols <<"----------"<<endl;

	myfile <<"---------- write ddr---------"<< endl;
	myfile <<"---------- "<< write_ddr_cnt <<" ----------"<<endl;

	myfile <<"---------- read ddr----------"<< endl;
	myfile <<"---------- "<< read_ddr_cnt <<" ----------"<<endl;

    myfile <<"---------- After whole cross correlation, simulation time is ----------" << endl;
	myfile <<"---------- "<< offset <<" ----------"<<endl;
	myfile << endl;
    myfile.close();
	
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

void Soft::write_bram(sc_dt::uint64 addr,char * val,int length)
{
	pl_t pl;

	/*
		Write to BRAM requires 11 cycles (modeled as 9 cycles here, 1 cycle in interconnect and 1 in BRAM) 
		and then for each next 4 bytes (bus_width), only 1 cycle (modeled in BRAM)
	*/
	offset += sc_core::sc_time((9+1)*DELAY , sc_core::SC_NS);
	
	//div_four is number how much is missing for lenght to be divisible with 4.
	int div_four = 0;
	unsigned char buf1[4];
	
	while(length % 4)
	{
		length++;
		div_four ++;
	}

	unsigned char *buf = new unsigned char[length];
	
	for(int i = 0; i < length - div_four; i++)
	{
    	buf[i] = Convert_to_UnsignedC(val[i]);
   	}
  
    for(int i = length - div_four; i < length; i++)
    {
    	buf[i] = 0;
    }


    pl.set_data_length(BUS_WIDTH);
    
    for(int j = 0; j < length / 4; j++)
    {
    	int l = 0;
    	
    	for(int p = 4 * j; p < 4 + j *4; p++)
    	{
    		buf1[l] = buf[p];
    		l++;
    	}
    	read_ddr_cnt += 4;
		pl.set_address(VP_ADDR_BRAM_L + addr + j *4);
		pl.set_data_ptr(buf1);
		pl.set_command(tlm::TLM_WRITE_COMMAND);
		pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
		interconnect_socket->b_transport(pl,offset);
		//cout<<"----------------------"<<endl;
    }
    
    /*
    	In real system, all these packages will be sent at once.
    	Here is implemented this way for simplicity of bram.cpp
    */

}

void Soft::read_bram(sc_dt::uint64 addr, int *stripe_resp, int *stripe_sum1, int *stripe_sum2, int length)
{
	offset += sc_core::sc_time((9+1)*DELAY , sc_core::SC_NS);		
	
	//unsigned char *buf = new unsigned char[length];

	// Check if reading from bram needs extra cycles (like writing)
    pl_t pl;
    unsigned char buf[4];
    int order = 1; // 1 - resp, 2 - sum1, 3 - sum2
    int n = 0;
    
	for(int i = 0; i < length; i++)
	{
		write_ddr_cnt += 4; 
    	pl.set_address(VP_ADDR_BRAM_L + addr + i*4);
    	pl.set_data_length(BUS_WIDTH); //reading 4 bytes (double)
		pl.set_data_ptr(buf);
		pl.set_command(tlm::TLM_READ_COMMAND);
		pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
		interconnect_socket->b_transport(pl,offset);
		
		if (order == 1)
		{
			stripe_resp[n] = toInt(buf);
			order++;
			continue;
		}	
		else if (order == 2)
		{
			stripe_sum1[n] = toInt(buf);
			order++;
			continue;
		}
		else if (order == 3)
		{
			stripe_sum2[n] = toInt(buf);
			order = 1;
			n++;
			continue;
		}
		
	}
}

int Soft::read_hard(sc_dt::uint64 addr)
{
    pl_t pl;
    unsigned char buf[8];
    pl.set_address(VP_ADDR_IP_HARD_L + addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_READ_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    sc_core::sc_time offset = sc_core::SC_ZERO_TIME;
    interconnect_socket->b_transport(pl,offset);
    return toInt(buf);
}

void Soft::write_hard(sc_dt::uint64 addr,int val)
{
    pl_t pl;
    unsigned char buf[4];
    toUchar(buf,val); 	
    pl.set_address(VP_ADDR_IP_HARD_L + addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_WRITE_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    interconnect_socket->b_transport(pl,offset);
}
