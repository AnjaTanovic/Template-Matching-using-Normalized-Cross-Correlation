#include "soft.hpp"
#include <unistd.h>

//using namespace chrono;

int read_ddr_cnt = 0;
int write_ddr_cnt = 0;
int picture_num = 0;
char** data_string;
int argc;
int h ;

SC_HAS_PROCESS(Soft);

Soft::Soft(sc_core::sc_module_name name,char** strings, int argv) : 
	sc_module(name), 
	offset(sc_core::SC_ZERO_TIME),
	interconnect_socket("interconnect_socket")
{
    SC_THREAD(soft);
	interconnect_socket(*this);
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
		//reading dimensions_i.txt, image_i_j.txt, template_i_j.txt (1 + 3 + 3)
		for (h = 1; h < argc; h += 7)
		{
			cout << "Software started working" << endl;
	
			FILE* myfile; 
         
            //reading dimensions	
			myfile = fopen(data_string[h], "r"); 
			if (!myfile)
            {  
				cout << "File dimensions can't be read" << endl;
				cout << data_string[h] << endl;  
				exit(-1); 
            }

			int dimensions[4];
			int img_rows;
			int img_cols;
			int tmpl_rows;
			int tmpl_cols;

			int k = 0;

			while (fscanf(myfile, "%d", &dimensions[k]) == 1)  
			{ 
				if (k > 4)
				cout << "Dimensions file is not correct" << endl;

				k++;
			} 
			fclose(myfile);

			img_rows = dimensions[0];
			img_cols = dimensions[1];
			tmpl_rows = dimensions[2];
			tmpl_cols = dimensions[3];

			//reading image
			int *img_b = new int[img_rows * img_cols];
			int *img_g = new int[img_rows * img_cols];
			int *img_r = new int[img_rows * img_cols];

			myfile = fopen(data_string[h + 1], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl; 
				cout << data_string[h + 1] << endl; 
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &img_b[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);

			myfile = fopen(data_string[h + 2], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl;
				cout << data_string[h + 2] << endl;  
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &img_g[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);
			
			myfile = fopen(data_string[h + 3], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl; 
				cout << data_string[h + 3] << endl; 
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &img_r[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);

			//reading template
			int *tmpl_b = new int[tmpl_rows * tmpl_cols];
			int *tmpl_g = new int[tmpl_rows * tmpl_cols];
			int *tmpl_r = new int[tmpl_rows * tmpl_cols];

			myfile = fopen(data_string[h + 4], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl; 
				cout << data_string[h + 4] << endl; 
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &tmpl_b[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);

			myfile = fopen(data_string[h + 5], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl; 
				cout << data_string[h + 5] << endl; 
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &tmpl_g[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);

			myfile = fopen(data_string[h + 6], "r");
			if (!myfile)
            {  
				cout << "File can't be read" << endl; 
				cout << data_string[h + 6] << endl; 
				exit(-1); 
            }

			k = 0;
			
			while (fscanf(myfile, "%d", &tmpl_r[k]) == 1) 
			{
				k++;
			}
			fclose(myfile);


			//Template matching operation starts
			cout << "Cross correlation is about to start" << endl;
	
			double *res = new double[(img_rows - tmpl_rows)*(img_cols - tmpl_cols)]; //result matrix
			int stripe = 0;
			int ch = 0; 
			
			double sum = 0;
			int sqrt_o = 0;
			
			int *stripe_resp = new int[img_cols - tmpl_cols];
			int *stripe_sum1 = new int[img_cols - tmpl_cols];
			int stripe_sum2;
    
			//flags
			bool done = 0;
			bool new_ch = 1;
			int ready = 1;
			bool need_start = 0; //if this flag is 1 it means that we need to send start bit to hard 
			bool calc_stripe = 0; //for sqrt and division
			    
			//Reset system
			wait(DELAY/2, SC_NS);
			write_hard(RESET,1);
			wait(5*DELAY, SC_NS);
			write_hard(RESET,0);
			wait(DELAY, SC_NS);

			//Initialize registers for dimensions
			cout << tmpl_rows << endl;
			write_hard(ADDR_TMPL_ROWS, tmpl_rows); 
			cout << tmpl_cols << endl;
			write_hard(ADDR_TMPL_COLS, tmpl_cols);
			cout << img_cols << endl;
			write_hard(ADDR_IMG_COLS, img_cols);
    
			cout << "Initialisation done by software" << endl;
	
			while (!done)
			{
			
				if(new_ch)
				{
					//tmpl_bram defines template in bram.
					char *tmpl_bram = new char[tmpl_rows * tmpl_cols];
					
					/*
					Write to BRAM requires 11 cycles (modeled as 9 cycles here, 1 cycle in interconnect and 1 in BRAM) 
					and then for each byte, only 1 cycle (modeled in BRAM)
					*/
					offset += sc_core::sc_time((9+1)*DELAY , sc_core::SC_NS);

					for (int j = 0; j < tmpl_rows; j++)
					{
						for (int k = 0; k < tmpl_cols; k++)
						{
							if (ch == 0)
								tmpl_bram[j * tmpl_cols + k] = tmpl_b[j * tmpl_cols + k];
							else if (ch == 1)
								tmpl_bram[j * tmpl_cols + k] = tmpl_g[j * tmpl_cols + k];
							else 
								tmpl_bram[j * tmpl_cols + k] = tmpl_r[j * tmpl_cols + k];

							//Write pixel of template in bram (one color)
							write_bram(BRAM_TMPL, j * tmpl_cols + k, tmpl_bram[j * tmpl_cols + k]); 
						}
					}

					delete[] tmpl_bram;
					
					new_ch = 0;
				}
					
				if (ready)
				{
					// stripe_b is used to change Mat to char*, defines stripe of image in bram
					char *stripe_b = new char[tmpl_rows * img_cols];
				   				
					/*
					Write to BRAM requires 11 cycles (modeled as 9 cycles here, 1 cycle in interconnect and 1 in BRAM) 
					and then for each byte, only 1 cycle (modeled in BRAM)
					*/
					offset += sc_core::sc_time((9+1)*DELAY , sc_core::SC_NS);

					for (int j = 0; j < tmpl_rows; j++)
					{
						for (int k = 0; k < img_cols; k++)
						{
							if (ch == 0)
								stripe_b[j * img_cols + k]= img_b[(j + stripe) * img_cols + k];
							else if (ch == 1)
								stripe_b[j * img_cols + k]= img_g[(j + stripe) * img_cols + k];
							else if (ch == 1)
								stripe_b[j * img_cols + k]= img_r[(j + stripe) * img_cols + k];
							
							//Write pixel of template in bram (one color)
							write_bram(BRAM_IMG, j * img_cols + k, stripe_b[j * img_cols + k]); 
						}
					}
					
					delete[] stripe_b;
					
					need_start = 1;
			   	}

				//Hard starts cross correlation (start will reset ready register in hard)
				if (need_start)
				{	
					write_hard(ADDR_START,1);
					write_hard(ADDR_START,0);
					need_start = 0;
				}
				
				/*
				//Waiting for hard to reset ready (need_new_stripe) (when it is 0, exit loop)
				while(ready)
				{
						ready = read_hard(ADDR_READY);
						wait(DELAY/4, SC_NS);
						if (!ready)
						    write_hard(ADDR_START,0);
				}
				*/

				if (calc_stripe)
				{
					// Checking time needed to do sqrt and div 
					//auto start = steady_clock::now();
					
					for (int j = 0; j < img_cols - tmpl_cols; j++)
					{
					
						sum = (double)stripe_sum1[j] * (double)stripe_sum2;
						sqrt_o = (int)sqrt(sum);
						
						if ((stripe - 1) >= 0)
							res[(stripe - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;	
						else
							res[(img_rows - tmpl_rows - 1)*(img_cols-tmpl_cols) + j] += (double)stripe_resp[j] / sqrt_o;
							
						
					}
					//auto end = steady_clock::now();
					
					//cout << "Required time for sqrt and div is: " << duration_cast<nanoseconds>(end - start).count() << " ns" << endl;
					
				}

				if (ch == 3)
				{
					done = 1; //exit loop
				}
				
				//Checking if hard needs new stripe
				ready = read_hard(ADDR_READY);
				wait(DELAY, SC_NS);

				if(ready)
				{	
					/*
					Read from BRAM requires 11 cycles (modeled as 9 cycles here, 1 cycle in interconnect and 1 in BRAM) 
					and then for each byte, only 1 cycle (modeled in BRAM)
					*/
					offset += sc_core::sc_time((9+1)*DELAY , sc_core::SC_NS);

					
					for (int j = 0; j < img_cols - tmpl_cols; j++)
					{
						read_bram(BRAM_RESP, j, stripe_resp);
						read_bram(BRAM_SUM1, j, stripe_sum1);
					}
					stripe_sum2 = read_hard(ADDR_SUM2);
					
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
					
					//Reset system
					wait(DELAY, SC_NS);
					write_hard(RESET,1);
					write_hard(RESET,0);
						
				}	
			}
	
			cout<<"After whole cross correlation, simulation time is:" << offset << endl;
	
			delete[] stripe_resp;
			delete[] stripe_sum1;

			delete[] img_b;
			delete[] img_g;
			delete[] img_r;
			delete[] tmpl_b;
			delete[] tmpl_g;
			delete[] tmpl_r;

			// print read and write ddr throughput
			//cout <<"write ddr: "<<write_ddr_cnt << endl;
			//cout <<"read ddr: "<<read_ddr_cnt << endl;
			// Printing data for frame. 
	
			/*
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
			*/

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

    		//Storing result in file
			char result_name[35]= "postprocessing/result_";
			string num_str = to_string(picture_num);
			char const *tmp = num_str.c_str();
			
			strcat(result_name,tmp);
			strcat(result_name,".txt");
				
			myfile = fopen(result_name, "w");
			if (!myfile)
            {  
				cout << "File can't be created" << endl; 
				exit(-1); 
            }
			
			for (int j = 0; j < img_rows - tmpl_rows; j++)
			{
				for (int k = 0; k < img_cols - tmpl_cols; k++)
				{
					fprintf(myfile, "%d\n", (int)res[j*(img_cols - tmpl_cols) + k]);
				}
			}

			fclose(myfile);	

    		delete[] res;

			picture_num++;
		}
}

void Soft::write_bram(int bram_number, sc_dt::uint64 addr,char val)
{
	//bram number: BRAM_TMPL, BRAM_IMG
	
	pl_t pl;
    unsigned char buf[4];
    toUchar(buf,val); 	
    pl.set_address(VP_ADDR_HARD_L + bram_number + addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_WRITE_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    interconnect_socket->b_transport(pl,offset);
    
    //In hard module, address and we signals are sent
}

void Soft::read_bram(int bram_number, sc_dt::uint64 addr, int *stripe)
{	
	//bram number: BRAM_RESP, BRAM_SUM1
			
    pl_t pl;
    unsigned char buf[4];
    write_ddr_cnt += 1; 
	pl.set_address(VP_ADDR_HARD_L + bram_number + addr);
	pl.set_data_length(BUS_WIDTH); //reading 4 bytes (double) ??? mzd je sad okej 1
	pl.set_data_ptr(buf);
	pl.set_command(tlm::TLM_READ_COMMAND);
	pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
	interconnect_socket->b_transport(pl,offset);
		
	stripe[addr] = toInt(buf);
	
	//In hard module, address and we signals are sent
}

int Soft::read_hard(sc_dt::uint64 addr)
{
	//reading ready or sum2
    pl_t pl;
    unsigned char buf[4];
    pl.set_address(VP_ADDR_HARD_L + addr);
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
    pl.set_address(VP_ADDR_HARD_L + addr);
    pl.set_data_length(1);
    pl.set_data_ptr(buf);
    pl.set_command(tlm::TLM_WRITE_COMMAND);
    pl.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    interconnect_socket->b_transport(pl,offset);
}

tlm_sync_enum Soft::nb_transport_bw(pl_t& pl, ph_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

void Soft::invalidate_direct_mem_ptr(uint64  start, uint64  end)
{
	
}
