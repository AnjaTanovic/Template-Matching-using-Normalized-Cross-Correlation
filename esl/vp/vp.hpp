#ifndef VP_HPP_
#define VP_HPP_

#include <systemc>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
//#include "functions.hpp"
//#include "addr.hpp"
#include "soft.hpp"
#include "interconnect.hpp"
#include "ip_hard.hpp"
#include "bram.hpp"

class Vp :  public sc_core::sc_module
{
	public:
		Vp(sc_core::sc_module_name name,char** strings, int argv);
		~Vp();

	protected:
		Soft soft;
		Interconnect interconnect;
		Ip_hard ip_hard;
		Bram bram;		
};

#endif // VP_HPP_
