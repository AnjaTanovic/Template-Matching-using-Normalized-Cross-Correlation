`ifndef ENV2_SV
 `define ENV2_SV

class env2 extends uvm_env;

   agent2 cross_agent2;
   cross_corr_config2 cfg;
   scoreboard2 scbd2;

   virtual interface cross_corr_if vif;
   `uvm_component_utils (env2)

   function new(string name = "env2", uvm_component parent = null);
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
      uvm_config_db#(cross_corr_config2)::set(this, "cross_agent2", "cross_corr_config2", cfg);
      uvm_config_db#(cross_corr_config2)::set(this, "scbd2", "cross_corr_config2", cfg);
      uvm_config_db#(virtual cross_corr_if)::set(this, "cross_agent2", "cross_corr_if", vif);
      /*****************************************************************/
      cross_agent2 = agent2::type_id::create("cross_agent2", this);

      //Scoreboard dodat
      scbd2 = scoreboard2::type_id::create("scbd2", this);
      	

   endfunction : build_phase

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      cross_agent2.mon2.item_collected_port.connect(scbd2.item_collected_import);
   endfunction : connect_phase

endclass : env2

`endif

