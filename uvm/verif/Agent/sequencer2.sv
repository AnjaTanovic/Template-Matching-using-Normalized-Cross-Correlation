`ifndef SEQUENCER2_SV
`define SEQUENCER2_SV

class sequencer2 extends uvm_sequencer#(seq_item);

   `uvm_component_utils(sequencer2)

   // configuration
   cross_corr_config2 cfg;
   
   function new(string name = "sequencer2", uvm_component parent = null);
      super.new(name,parent);
      
      if(!uvm_config_db#(cross_corr_config2)::get(this, "", "cross_corr_config2", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
      /*****************************************************************/

   endfunction

endclass : sequencer2

`endif


