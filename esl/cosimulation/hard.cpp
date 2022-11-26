#include "hard.hpp"
#include <tlm>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;

SC_HAS_PROCESS(Hard);

Hard::Hard(sc_module_name name):
	sc_module(name),
	interconnect_socket("interconnect_socket"),
	dut("dut"),
	clk("clk", DELAY, sc_core::SC_NS)
	
{
	interconnect_socket(*this);
	dut.clk(clk.signal());
	dut.reset(reset);
	dut.start(start);
	dut.tmpl_cols_i(tmpl_cols_i);
	dut.tmpl_rows_i(tmpl_rows_i);
	dut.img_cols_i(img_cols_i);
	dut.ready_o(ready_o);
	dut.sum1_data_o(sum1_data_o);
	dut.sum2_data_o(sum2_data_o);
	dut.resp_data_o(resp_data_o);
	dut.img_data_i(img_data_i);
	dut.tmpl_data_i(tmpl_data_i);
	dut.img_addr_i(img_addr_i);
	dut.tmpl_addr_i(tmpl_addr_i);
	dut.sum1_addr_i(sum1_addr_i);
	dut.resp_addr_i(resp_addr_i);
	dut.we_img_i(we_img_i);
	dut.we_tmpl_i(we_tmpl_i);
	
	SC_REPORT_INFO("Hard", "Constructed.");
}

Hard::~Hard()
{
	SC_REPORT_INFO("Hard", "Destroyed.");
}


void Hard::b_transport(pl_t &pl, sc_core::sc_time &offset)
{
	//cout <<" package recieved" << endl;
	
	tlm::tlm_command cmd = pl.get_command();
 	sc_dt::uint64 addr = pl.get_address();
 	sc_dt::uint64 taddr;
	unsigned int len = pl.get_data_length();
 	unsigned char *buf = pl.get_data_ptr();
 	pl.set_response_status( tlm::TLM_OK_RESPONSE );
 	
	sc_int<32> res_int;
	int res;

	switch(cmd)
 	{	
 	case tlm::TLM_WRITE_COMMAND:
      	if (addr == ADDR_TMPL_ROWS)
		{
			tmpl_rows_i.write(static_cast<sc_lv<8>>(toInt(buf)));
			wait(DELAY, SC_NS);
			cout << "tmpl_rows = " << tmpl_rows_i << endl;
		}
		else if (addr == ADDR_TMPL_COLS)
		{	
			tmpl_cols_i.write(static_cast<sc_lv<8>>(toInt(buf)));
			wait(DELAY, SC_NS);
			cout << "tmpl_cols = " << tmpl_cols_i << endl;
		}
		else if (addr == ADDR_IMG_COLS)
		{
			img_cols_i.write(static_cast<sc_lv<10>>(toInt(buf)));
			wait(DELAY, SC_NS);
			cout << "img_cols = " << img_cols_i << endl;
		}
		else if (addr == ADDR_START)
		{
			start.write(static_cast<sc_logic>(toInt(buf)));
			wait(DELAY, SC_NS);
			cout << "Module started" << endl;
		}
		else if (addr >= BRAM_TMPL && addr < BRAM_IMG)
		{	
			taddr = addr - BRAM_TMPL; 
			tmpl_data_i.write(toInt(buf));
			tmpl_addr_i.write(taddr);
			we_tmpl_i.write(15); //1111
			wait(DELAY, SC_NS);
			cout << "Template pixel sent to bram" << endl;
		}
		else if (addr >= BRAM_IMG && addr < BRAM_RESP)
		{
			taddr = addr - BRAM_IMG;
			img_data_i.write(toInt(buf));
			img_addr_i.write(taddr);
			we_img_i.write(15); //1111
			wait(DELAY, SC_NS);
			cout << "Image pixel sent to bram" << endl;
		}
		else if (addr == RESET)
		{
			reset.write(static_cast<sc_logic>(toInt(buf)));
			wait(DELAY, SC_NS);
			cout << "Module reset " << endl;
		}
		else
		{
			pl.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
			cout << "Wrong address" << endl;
		}
      		
      		break;
	case tlm::TLM_READ_COMMAND:
		if (addr == ADDR_READY)
		{
			if (ready_o.read() == '0')
				toUchar(buf, 0);
			if (ready_o.read() == '1')
				toUchar(buf, 1);
		}
		else if (addr >= BRAM_RESP && addr < BRAM_SUM1)
		{
			taddr = addr - BRAM_RESP; 
			resp_addr_i.write(taddr);
			wait(DELAY, sc_core::SC_NS);
			res_int = static_cast <sc_int<32>>(resp_data_o.read());
			res = res_int.to_int();
			toUchar(buf, res);
			cout << "Resp taken from bram" << endl;
		}
		else if (addr >= BRAM_SUM1 && addr < BRAM_END)
		{
			taddr = addr - BRAM_SUM1; 
			sum1_addr_i.write(taddr);
			wait(DELAY, sc_core::SC_NS);
			res_int = static_cast <sc_int<32>>(sum1_data_o.read());
			res = res_int.to_int();
			toUchar(buf, res);
			cout << "Sum1 taken from bram" << endl;
		}
		else if (addr == ADDR_SUM2)
		{
			res_int = static_cast <sc_int<32>>(sum2_data_o.read());
			wait(DELAY, sc_core::SC_NS);
			res = res_int.to_int();
			toUchar(buf, res);
			cout << "Sum2 taken from bram" << endl;
		}
		else
		{
			pl.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
		}
		
		break;
	default:
		pl.set_response_status( tlm::TLM_COMMAND_ERROR_RESPONSE );
		cout << "Wrong command" << endl;
	}
	
	offset += sc_core::sc_time(DELAY, sc_core::SC_NS);
}	

tlm_sync_enum Hard::nb_transport_fw(pl_t& pl, ph_t& phase, sc_time& offset)
{
	return TLM_ACCEPTED;
}

bool Hard::get_direct_mem_ptr(pl_t& pl, tlm_dmi& dmi)
{
	return true;
}

unsigned int Hard::transport_dbg(pl_t &)
{
	return 0;
}
