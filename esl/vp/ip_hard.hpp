#ifndef _IP_HARD_H_
#define _IP_HARD_H_

#define SC_INCLUDE_FX
#include <systemc>
#include <sysc/datatypes/fx/sc_fixed.h>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "addr.hpp"
#include "functions.hpp"

using namespace std;

class Ip_hard :
	public sc_core::sc_module
{
public:
	Ip_hard(sc_core::sc_module_name);
	~Ip_hard();
	
	tlm_utils::simple_target_socket<Ip_hard> interconnect_socket;
	tlm_utils::simple_initiator_socket<Ip_hard> bram_socket;

protected:
	pl_t pl;
	sc_core::sc_time offset;
	
	//input parametres
	sc_dt::sc_uint<1> start;
	sc_dt::sc_uint<8> tmpl_rows;
	sc_dt::sc_uint<8> tmpl_cols;
	sc_dt::sc_uint<10> img_cols;
	
	//calculated variables in loop (sum1, sum2, resp)
	
	sc_dt::sc_int <32> sum1;
	sc_dt::sc_int <32> sum2;
	sc_dt::sc_int <32> resp;
	
	//ouput signals
	sc_dt::sc_uint<1> ready;
 
	void b_transport(pl_t&, sc_core::sc_time&);	
	void crossCorrelation(sc_core::sc_time &);
	
	char read_bram(sc_dt::sc_uint<64> addr);
	void write_bram(sc_dt::sc_uint<64> addr, sc_dt::sc_int <32> val);
	
};

#endif
