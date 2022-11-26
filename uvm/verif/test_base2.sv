`ifndef TEST_BASE2_SV
`define TEST_BASE2_SV

class test_base2 extends uvm_test;

   env2 cross_env2;
   cross_corr_config2 cfg;

   `uvm_component_utils(test_base2)

   function new(string name = "test_base2", uvm_component parent = null);
      super.new(name,parent);
   endfunction : new

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);
      cfg = cross_corr_config2::type_id::create("cfg"); 
      cfg.randomize(); 
      cfg.randomize_config(); //randomize files
      uvm_config_db#(cross_corr_config2)::set(this, "cross_env2", "cross_corr_config2", cfg);      
      cross_env2 = env2::type_id::create("cross_env2", this);      
   endfunction : build_phase

   function void end_of_elaboration_phase(uvm_phase phase);
      super.end_of_elaboration_phase(phase);
      uvm_top.print_topology();
   endfunction : end_of_elaboration_phase

endclass : test_base2

`endif

