#ifndef VP_HPP_
#define VP_HPP_

#include <systemc>
//#include <tlm_utils/simple_initiator_socket.h>
//#include <tlm_utils/simple_target_socket.h>
#include "functions.hpp"
#include "addr.hpp"
#include "soft.hpp"
#include "interconnect.hpp"
#include "hard.hpp"

class Vp :  public sc_core::sc_module
{
	public:
		Vp(sc_core::sc_module_name name,char** strings, int argv);
		~Vp();

	protected:
		Soft soft;
		Interconnect interconnect;
		Hard hard;		
};

#endif // VP_HPP_
