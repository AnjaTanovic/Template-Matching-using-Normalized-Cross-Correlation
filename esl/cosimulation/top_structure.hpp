#ifndef _TOP_STRUCTURE_HPP_
#define _TOP_STRUCTURE_HPP_

#include <systemc>

class top_structure : public sc_core::sc_foreign_module
{
public:
	top_structure(sc_core::sc_module_name name) :
		sc_core::sc_foreign_module(name),
		clk("clk"),
		reset("reset"),
		start("start"),
		tmpl_cols_i("tmpl_cols_i"),
		tmpl_rows_i("tmpl_rows_i"),
		img_cols_i("img_cols_i"),
		ready_o("ready_o"),
		sum1_data_o("sum1_data_o"),
		sum2_data_o("sum2_data_o"),
		resp_data_o("resp_data_o"),
		img_data_i("img_data_i"),
		tmpl_data_i("tmpl_data_i"),
		img_addr_i("img_addr_i"),
		tmpl_addr_i("tmpl_addr_i"),
		sum1_addr_i("sum1_addr_i"),
		resp_addr_i("resp_addr_i"),
		we_img_i("we_img_i"),
		we_tmpl_i("we_tmpl_i")
	{

	}

	sc_core::sc_in< bool > clk;
	sc_core::sc_in< sc_dt::sc_logic > reset;
	sc_core::sc_in< sc_dt::sc_logic > start;
	sc_core::sc_in< sc_dt::sc_lv<8> > tmpl_cols_i;
	sc_core::sc_in< sc_dt::sc_lv<8> > tmpl_rows_i;
	sc_core::sc_in< sc_dt::sc_lv<10> > img_cols_i;
	sc_core::sc_out< sc_dt::sc_logic > ready_o;
	sc_core::sc_out< sc_dt::sc_lv<32> > sum1_data_o;
	sc_core::sc_out< sc_dt::sc_lv<32> > sum2_data_o;
	sc_core::sc_out< sc_dt::sc_lv<32> > resp_data_o;
	sc_core::sc_in< sc_dt::sc_lv<8> > img_data_i;
	sc_core::sc_in< sc_dt::sc_lv<8> > tmpl_data_i;
	sc_core::sc_in< sc_dt::sc_lv<10 + 8> > img_addr_i;
	sc_core::sc_in< sc_dt::sc_lv<8 + 8> > tmpl_addr_i;
	sc_core::sc_in< sc_dt::sc_lv<10> > sum1_addr_i;
	sc_core::sc_in< sc_dt::sc_lv<10> > resp_addr_i;
	sc_core::sc_in< sc_dt::sc_lv<4> > we_img_i;
	sc_core::sc_in< sc_dt::sc_lv<4> > we_tmpl_i;

	const char* hdl_name() const { return "top_structure"; }
};

#endif
