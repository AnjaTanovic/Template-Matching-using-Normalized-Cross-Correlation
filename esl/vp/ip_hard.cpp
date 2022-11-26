#include "ip_hard.hpp"
#include <tlm>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;

SC_HAS_PROCESS(Ip_hard);

Ip_hard::Ip_hard(sc_module_name name):
	sc_module(name),
	ready(1)
	
{
	interconnect_socket.register_b_transport(this, &Ip_hard::b_transport);
	
	SC_REPORT_INFO("Hard", "Constructed.");
}

Ip_hard::~Ip_hard()
{
	SC_REPORT_INFO("Ip_hard", "Destroyed.");
}


void Ip_hard::b_transport(pl_t &pl, sc_core::sc_time &offset)
{
	//cout <<" package recieved" << endl;
	
	tlm::tlm_command cmd = pl.get_command();
 	sc_dt::uint64 addr = pl.get_address();
	unsigned int len = pl.get_data_length();
 	unsigned char *buf = pl.get_data_ptr();
 	pl.set_response_status( tlm::TLM_OK_RESPONSE );
 	
	switch(cmd)
 	{	
 	case tlm::TLM_WRITE_COMMAND:
      		switch(addr)
        	{
			case ADDR_TMPL_ROWS:
			  tmpl_rows = toInt(buf);  
			  cout << "tmpl_rows = " << tmpl_rows << endl;
			  break;
			case ADDR_TMPL_COLS:
			  tmpl_cols = toInt(buf);
			  cout << "tmpl_cols = " << tmpl_cols << endl;
			  break;
			case ADDR_IMG_COLS:
			  img_cols = toInt(buf);
			  cout << "img_cols = " << img_cols << endl;
			  break;
			case ADDR_START:
			  start = toInt(buf);
			  cout << "start bit" << endl;
			  crossCorrelation(offset);
			  break;
			default:
			  pl.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
			  cout << "Wrong address" << endl;
		}
      		
      		break;
	case tlm::TLM_READ_COMMAND:
		switch(addr)
		{
		case ADDR_READY:
		  toUchar(buf, ready);
		  break;
		default:
		  pl.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
		}
		
		break;
	default:
		pl.set_response_status( tlm::TLM_COMMAND_ERROR_RESPONSE );
		cout << "Wrong command" << endl;
	}
	
	offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
}

void Ip_hard::crossCorrelation(sc_core::sc_time &system_offset)
{
	cout << "Cross Correlation started" << endl;
	
	pl_t pl;
	res_type res;

	double bram_cnt = 0; //variable used for incrementing places in memory (one pixel of double is taking 4 places in memory)
	
	sc_dt::sc_int <8> img_pixel; 
	sc_dt::sc_int <8> tmpl_pixel;
	
	if (start == 1 && ready == 1)        //Ip core is started
	{
		ready = 0;
		offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
	}
	
	//condition start = 1 & ready = 0 will never happen in our code
	
	else if (start == 0 && ready == 0)     //Start bit is returned to 0, start of working
	{
		//cout << "start = 0 && ready == 0" << endl;
		
		//Cross Correlation
		
		for (sc_dt::sc_uint<10> j = 0; j < img_cols - tmpl_cols; j++)
		{
			resp = 0;
			sum1 = 0;
			sum2 = 0;
			
			//The distance is calculated for each point of original image (i, j) 
			for (sc_dt::sc_uint<8> k = 0; k < tmpl_rows; k++)
			{
				for (sc_dt::sc_uint<8> m = 0; m < tmpl_cols; m++)
				{	
					tmpl_pixel = read_bram(k* tmpl_cols+m);
					img_pixel = read_bram(tmpl_rows*tmpl_cols   +   k*img_cols + j+m );
					
					resp += img_pixel * tmpl_pixel;
					sum1 += img_pixel * img_pixel; 
					sum2 += tmpl_pixel * tmpl_pixel;
					
					// These operations goes in parallel and it takes one cycle for each pixel
					offset += sc_core::sc_time(DELAY, sc_core::SC_NS);	
					
					//cout << img_pixel << " " << tmpl_pixel << endl;
					
				}
			}
			
			//sqrt and division operations are being calculated during the computation of next pixel, in software   
			
			//Store sum1, sum2 and resp in BRAM
			
			write_bram((tmpl_rows*tmpl_cols)    +    (tmpl_rows*img_cols)  + j + bram_cnt, resp);
			bram_cnt += 4;
			write_bram((tmpl_rows*tmpl_cols)    +    (tmpl_rows*img_cols)  + j + bram_cnt , sum1);
			bram_cnt += 4;
			write_bram((tmpl_rows*tmpl_cols)    +    (tmpl_rows*img_cols)  + j + bram_cnt, sum2);
			bram_cnt += 3;
			
			
		}
		ready = 1;
		
		//offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
		cout << "Cross stripe done" << endl;
    	}
	
}	

void Ip_hard::write_bram(sc_dt::sc_uint<64> addr, sc_dt::sc_int <32> val)
{
	pl_t pl;
	unsigned char buf[4];
	toUchar(buf,val);
	pl.set_address(addr);
	pl.set_data_length(BUS_WIDTH); 
	pl.set_data_ptr(buf);
	pl.set_command( tlm::TLM_WRITE_COMMAND );
	pl.set_response_status ( tlm::TLM_INCOMPLETE_RESPONSE );
	bram_socket->b_transport(pl, offset);
}

char Ip_hard::read_bram(sc_dt::sc_uint<64> addr)
{
	pl_t pl;
	unsigned char buf;
	pl.set_address(addr);
	pl.set_data_length(1); 
	pl.set_data_ptr(&buf);
	pl.set_command( tlm::TLM_READ_COMMAND );
	pl.set_response_status ( tlm::TLM_INCOMPLETE_RESPONSE );
	bram_socket->b_transport(pl, offset);
	return Convert_to_SigendC(buf);
}
