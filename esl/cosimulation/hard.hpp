#ifndef _HARD_H_
#define _HARD_H_

#define SC_INCLUDE_FX
#include <systemc>
//#include <sysc/datatypes/fx/sc_fixed.h>
#include <tlm>
//#include <tlm_utils/simple_target_socket.h>
//#include <tlm_utils/simple_initiator_socket.h>
#include "addr.hpp"
#include "functions.hpp"

#include "top_structure.hpp"

using namespace std;
using namespace tlm;

class Hard :
	public sc_core::sc_module,
	public tlm::tlm_fw_transport_if<>
{
public:
	Hard(sc_core::sc_module_name);
	~Hard();
	
	tlm::tlm_target_socket<> interconnect_socket;
	
	pl_t pl;
	sc_core::sc_time offset;

	void b_transport(pl_t&, sc_core::sc_time&);	

	tlm::tlm_sync_enum nb_transport_fw(pl_t&, ph_t&, sc_core::sc_time &);
	bool get_direct_mem_ptr(pl_t&, tlm::tlm_dmi&);
	unsigned int transport_dbg(pl_t &);

protected:
	
	top_structure dut;
	sc_core::sc_clock clk;
	
	sc_core::sc_signal< sc_dt::sc_logic > reset;
	sc_core::sc_signal< sc_dt::sc_logic > start;
	sc_core::sc_signal< sc_dt::sc_lv<8> > tmpl_cols_i;
	sc_core::sc_signal< sc_dt::sc_lv<8> > tmpl_rows_i;
	sc_core::sc_signal< sc_dt::sc_lv<10> > img_cols_i;
	sc_core::sc_signal< sc_dt::sc_logic > ready_o;
	sc_core::sc_signal< sc_dt::sc_lv<32> > sum1_data_o;
	sc_core::sc_signal< sc_dt::sc_lv<32> > sum2_data_o;
	sc_core::sc_signal< sc_dt::sc_lv<32> > resp_data_o;
	sc_core::sc_signal< sc_dt::sc_lv<8> > img_data_i;
	sc_core::sc_signal< sc_dt::sc_lv<8> > tmpl_data_i;
	sc_core::sc_signal< sc_dt::sc_lv<10 + 8> > img_addr_i;
	sc_core::sc_signal< sc_dt::sc_lv<8 + 8> > tmpl_addr_i;
	sc_core::sc_signal< sc_dt::sc_lv<10> > sum1_addr_i;
	sc_core::sc_signal< sc_dt::sc_lv<10> > resp_addr_i;
	sc_core::sc_signal< sc_dt::sc_lv<4> > we_img_i;
	sc_core::sc_signal< sc_dt::sc_lv<4> > we_tmpl_i;

};

#ifndef SC_MAIN
SC_MODULE_EXPORT(Hard)
#endif

#endif
