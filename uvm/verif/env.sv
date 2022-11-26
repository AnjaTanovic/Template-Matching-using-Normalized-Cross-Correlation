`ifndef ENV_SV
 `define ENV_SV

class env extends uvm_env;

   agent cross_agent;
   cross_corr_config cfg;
   scoreboard scbd;

   virtual interface cross_corr_if vif;
   `uvm_component_utils (env)

   function new(string name = "env", uvm_component parent = null);
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
      uvm_config_db#(cross_corr_config)::set(this, "cross_agent", "cross_corr_config", cfg);
      uvm_config_db#(cross_corr_config)::set(this, "scbd", "cross_corr_config", cfg);
      uvm_config_db#(virtual cross_corr_if)::set(this, "cross_agent", "cross_corr_if", vif);
      /*****************************************************************/
      cross_agent = agent::type_id::create("cross_agent", this);

      //Scoreboard dodat
      scbd = scoreboard::type_id::create("scbd", this);
      	

   endfunction : build_phase

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      cross_agent.mon.item_collected_port.connect(scbd.item_collected_import);
   endfunction : connect_phase

endclass : env

`endif

