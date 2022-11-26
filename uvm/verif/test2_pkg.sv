`ifndef TEST2_PKG_SV
`define TEST2_PKG_SV

package test2_pkg;

	import uvm_pkg::*;      // import the UVM library   
	`include "uvm_macros.svh" // Include the UVM macros

	import agent2_pkg::*;
	import seq2_pkg::*;
	import configurations2_pkg::*;   

	`include "scoreboard2.sv" 
	`include "env2.sv"   
	`include "test_base2.sv"
	`include "test_simple2.sv"  


endpackage : test2_pkg

 `include "cross_corr_if.sv"

`endif


