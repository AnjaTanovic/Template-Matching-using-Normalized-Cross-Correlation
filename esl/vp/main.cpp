#include "vp.hpp"
#include "addr.hpp"

using namespace sc_core;


int sc_main(int argc, char* argv[])
{
    //tr = Treshold_convert(argv[1]);
    
    Vp vp("Virtual Platform", argv, argc);
    sc_start(1000, SC_NS);
    
    /*Vp vp("Virtual Platform");
    sc_start(1000, SC_NS);
    */
    return 0;
}  
