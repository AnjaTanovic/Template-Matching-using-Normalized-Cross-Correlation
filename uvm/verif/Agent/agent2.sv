`ifndef AGENT2_SV
`define AGENT2_SV

class agent2 extends uvm_agent;

   // components
   driver drv;
   sequencer2 seqr2;
   monitor2 mon2;
   
   virtual interface cross_corr_if vif;
   
   // configuration
   cross_corr_config2 cfg;
     
   `uvm_component_utils_begin (agent2)
        `uvm_field_object(cfg, UVM_DEFAULT)
        `uvm_field_object(drv, UVM_DEFAULT)
        `uvm_field_object(seqr2, UVM_DEFAULT)
        `uvm_field_object(mon2, UVM_DEFAULT)
   `uvm_component_utils_end

   function new(string name = "agent2", uvm_component parent = null);
      super.new(name,parent);
   endfunction

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);
      /************Geting from configuration database*******************/
      if (!uvm_config_db#(virtual cross_corr_if)::get(this, "", "cross_corr_if", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
      
      if(!uvm_config_db#(cross_corr_config2)::get(this, "", "cross_corr_config2", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
      /*****************************************************************/
      
      /************Setting to configuration database********************/
      uvm_config_db#(cross_corr_config2)::set(this, "mon2", "cross_corr_config2", cfg);
      uvm_config_db#(cross_corr_config2)::set(this, "seqr2", "cross_corr_config2", cfg);
      uvm_config_db#(virtual cross_corr_if)::set(this, "*", "cross_corr_if", vif);
      /*****************************************************************/
      
      mon2 = monitor2::type_id::create("mon2", this);
      if(cfg.is_active == UVM_ACTIVE) begin
         drv = driver::type_id::create("drv", this);
         seqr2 = sequencer2::type_id::create("seqr2", this);
      end
   endfunction : build_phase

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      if(cfg.is_active == UVM_ACTIVE) begin
         drv.seq_item_port.connect(seqr2.seq_item_export);
      end
   endfunction : connect_phase

endclass : agent2

`endif
