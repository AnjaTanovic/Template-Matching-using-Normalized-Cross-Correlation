`ifndef AGENT_SV
`define AGENT_SV

class agent extends uvm_agent;

   // components
   driver drv;
   sequencer seqr;
   monitor mon;
   
   virtual interface cross_corr_if vif;
   
   // configuration
   cross_corr_config cfg;
     
   `uvm_component_utils_begin (agent)
        `uvm_field_object(cfg, UVM_DEFAULT)
        `uvm_field_object(drv, UVM_DEFAULT)
        `uvm_field_object(seqr, UVM_DEFAULT)
        `uvm_field_object(mon, UVM_DEFAULT)
   `uvm_component_utils_end

   function new(string name = "agent", uvm_component parent = null);
      super.new(name,parent);
   endfunction

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);
      /************Geting from configuration database*******************/
      if (!uvm_config_db#(virtual cross_corr_if)::get(this, "", "cross_corr_if", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
      
      if(!uvm_config_db#(cross_corr_config)::get(this, "", "cross_corr_config", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
      /*****************************************************************/
      
      /************Setting to configuration database********************/
      uvm_config_db#(cross_corr_config)::set(this, "mon", "cross_corr_config", cfg);
      uvm_config_db#(cross_corr_config)::set(this, "seqr", "cross_corr_config", cfg);
      uvm_config_db#(virtual cross_corr_if)::set(this, "*", "cross_corr_if", vif);
      /*****************************************************************/
      
      mon = monitor::type_id::create("mon", this);
      if(cfg.is_active == UVM_ACTIVE) begin
         drv = driver::type_id::create("drv", this);
         seqr = sequencer::type_id::create("seqr", this);
      end
   endfunction : build_phase

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      if(cfg.is_active == UVM_ACTIVE) begin
         drv.seq_item_port.connect(seqr.seq_item_export);
      end
   endfunction : connect_phase

endclass : agent

`endif
