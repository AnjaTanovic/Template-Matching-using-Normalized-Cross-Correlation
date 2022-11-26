#include "vp.hpp"

Vp::Vp (sc_core::sc_module_name name, char** strings, int argv): 
	sc_module (name),
	soft("Soft",strings,argv),
	interconnect("Interconnect"),
	ip_hard("Ip_hard"),
	bram("Bram")

{
	soft.interconnect_socket.bind(interconnect.soft_socket);
	interconnect.bram_socket.bind(bram.bram_socket_1);
	interconnect.ip_hard_socket.bind(ip_hard.interconnect_socket);
	ip_hard.bram_socket.bind(bram.bram_socket_2);

	SC_REPORT_INFO("Virtual Platform", "Constructed.");
}

Vp::~Vp()
{
 	SC_REPORT_INFO("Virtual Platform", "Destroyed.");
}


