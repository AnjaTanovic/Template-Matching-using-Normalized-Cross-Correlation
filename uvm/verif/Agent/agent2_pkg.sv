`ifndef AGENT2_PKG
`define AGENT2_PKG

package agent2_pkg;
 
   import uvm_pkg::*;
   `include "uvm_macros.svh"

   //////////////////////////////////////////////////////////
   // include Agent components : driver,monitor,sequencer
   /////////////////////////////////////////////////////////
   import configurations2_pkg::*;   
   
   `include "seq_item.sv"
   `include "sequencer2.sv"
   `include "driver.sv"
   `include "monitor2.sv"
   `include "agent2.sv"

endpackage

`endif




