#include "vp.hpp"

Vp::Vp (sc_core::sc_module_name name, char** strings, int argv): 
	sc_module (name),
	soft("Soft",strings,argv),
	interconnect("Interconnect"),
	hard("Hard")

{
	soft.interconnect_socket(interconnect.soft_socket);
	interconnect.hard_socket(hard.interconnect_socket);

	SC_REPORT_INFO("Virtual Platform", "Constructed.");
}

Vp::~Vp()
{
 	SC_REPORT_INFO("Virtual Platform", "Destroyed.");
}


