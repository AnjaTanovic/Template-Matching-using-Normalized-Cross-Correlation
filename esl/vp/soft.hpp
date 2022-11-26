#pragma once 

#ifndef SOFT_HPP_ 
#define SOFT_HPP_

#include <iostream>
#include <string>
#include <systemc>
#include <fstream>

#include <charconv>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "addr.hpp"
#include "functions.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

class Soft : public sc_core::sc_module
{
	public:
	    Soft(sc_core::sc_module_name name,char** strings, int argv);
	    ~Soft();
	    tlm_utils::simple_initiator_socket<Soft> interconnect_socket;
	protected:
	    pl_t pl;
	    sc_core::sc_time offset;
	    void soft();
	    void templateMatching(const Mat bgr_img[], const Mat bgr_tmpl[],int tr, Mat& locations); 
	    void read_bram(sc_dt::uint64 addr, int *stripe_resp, int *stripe_sum1, int *stripe_sum2, int length);
		void write_bram(sc_dt::uint64 addr,char *val,int length);
	    int read_hard(sc_dt::uint64 addr);
	    void write_hard(sc_dt::uint64 addr,int val);
};
#endif
