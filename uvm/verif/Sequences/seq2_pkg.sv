`ifndef SEQ2_PKG_SV
`define SEQ2_PKG_SV

package seq2_pkg;

	import uvm_pkg::*;      // import the UVM library
	`include "uvm_macros.svh" // Include the UVM macros
	import agent2_pkg::seq_item;
	import agent2_pkg::sequencer2;
	`include "base_seq2.sv"
	`include "simple_seq2.sv"
     endpackage 
`endif
