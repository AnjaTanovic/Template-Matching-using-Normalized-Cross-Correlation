#pragma once 

#ifndef SOFT_HPP_ 
#define SOFT_HPP_

#include <iostream>
#include <string>
#include <systemc>
#include <tlm>

#include<sstream>
#include<fstream>
#include<iomanip>

//#include <charconv>
//#include <tlm_utils/simple_initiator_socket.h>
//#include <tlm_utils/simple_target_socket.h>

#include "addr.hpp"
#include "functions.hpp"

using namespace std;
using namespace tlm;

class Soft : 
	public sc_core::sc_module,
	public tlm::tlm_bw_transport_if<>
{
	public:
	    Soft(sc_core::sc_module_name name,char** strings, int argv);
	    ~Soft();
	    tlm::tlm_initiator_socket<> interconnect_socket;

		pl_t pl;
	    sc_core::sc_time offset;
	
		tlm::tlm_sync_enum nb_transport_bw(pl_t&, ph_t&, sc_core::sc_time &);
		void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
	protected:
	    
	    void soft();
	    void read_bram(int bram_number, sc_dt::uint64 addr, int *stripe);
		void write_bram(int bram_number, sc_dt::uint64 addr,char val);
	    int read_hard(sc_dt::uint64 addr);
	    void write_hard(sc_dt::uint64 addr,int val);
};

#endif
