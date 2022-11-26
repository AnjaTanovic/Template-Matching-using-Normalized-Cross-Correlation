#define SC_MAIN
//#include "vp.hpp"
#include <systemc>
#include <tlm>
#include "addr.hpp"
#include "functions.hpp"
#include "soft.hpp"
#include "interconnect.hpp"
#include "hard.hpp"

using namespace sc_core;


int sc_main(int argc, char* argv[])
{
    //Vp vp("Virtual Platform", argv, argc);

	Soft soft("Soft",argv,argc);
	Interconnect interconnect("Interconnect");
	Hard hard("Hard");

	soft.interconnect_socket(interconnect.soft_socket);
	interconnect.hard_socket(hard.interconnect_socket);

	SC_REPORT_INFO("Virtual Platform", "Constructed.");

    sc_start(1, SC_SEC);
    
    return 0;
}  
