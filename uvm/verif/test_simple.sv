`ifndef TEST_SIMPLE_SV
`define TEST_SIMPLE_SV

class test_simple extends test_base;

   `uvm_component_utils(test_simple)

   simple_seq cross_simple_seq;

   function new(string name = "test_simple", uvm_component parent = null);
      super.new(name,parent);
   endfunction : new

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);
      cross_simple_seq = simple_seq::type_id::create("cross_simple_seq");
   endfunction : build_phase

   task main_phase(uvm_phase phase);
      phase.raise_objection(this);
      cross_simple_seq.start(cross_env.cross_agent.seqr);
      phase.drop_objection(this);
   endtask : main_phase

endclass

`endif