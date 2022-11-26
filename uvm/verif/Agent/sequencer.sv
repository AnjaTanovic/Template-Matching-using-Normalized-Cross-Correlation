`ifndef SEQUENCER_SV
`define SEQUENCER_SV

class sequencer extends uvm_sequencer#(seq_item);

   `uvm_component_utils(sequencer)

   // configuration
   cross_corr_config cfg;
   
   function new(string name = "sequencer", uvm_component parent = null);
      super.new(name,parent);
      
      if(!uvm_config_db#(cross_corr_config)::get(this, "", "cross_corr_config", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
      /*****************************************************************/

   endfunction

endclass : sequencer

`endif


