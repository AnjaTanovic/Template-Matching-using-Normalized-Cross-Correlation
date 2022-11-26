#ifndef INTERCONNECT_HPP_
#define INTERCONNECT_HPP_

#include <iostream>
#include <systemc>
#include <tlm>
//#include <tlm_utils/simple_initiator_socket.h>
//#include <tlm_utils/simple_target_socket.h>
#include "addr.hpp"

using namespace tlm;
using namespace std;

class Interconnect : 
	public sc_core::sc_module,
	public tlm::tlm_bw_transport_if<>,
	public tlm::tlm_fw_transport_if<>
{
	public:
		Interconnect(sc_core::sc_module_name name);
		~Interconnect();

		tlm::tlm_initiator_socket<> hard_socket;
		tlm::tlm_target_socket<> soft_socket;
	 
		pl_t pl;
		sc_core::sc_time offset;
		void b_transport(pl_t &pl, sc_core::sc_time &offset);

		tlm::tlm_sync_enum nb_transport_bw(pl_t&, ph_t&, sc_core::sc_time &);
		void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64);
		
		tlm::tlm_sync_enum nb_transport_fw(pl_t&, ph_t&, sc_core::sc_time &);
		bool get_direct_mem_ptr(pl_t&, tlm::tlm_dmi&);
		unsigned int transport_dbg(pl_t &);
};

#endif // INTERCONNECT_HPP_
