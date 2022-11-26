`ifndef TEST_SIMPLE2_SV
`define TEST_SIMPLE2_SV

class test_simple2 extends test_base2;

   `uvm_component_utils(test_simple2)

   simple_seq2 cross_simple_seq2;

   function new(string name = "test_simple2", uvm_component parent = null);
      super.new(name,parent);
   endfunction : new

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);
      cross_simple_seq2 = simple_seq2::type_id::create("cross_simple_seq2");
   endfunction : build_phase

   task main_phase(uvm_phase phase);
      phase.raise_objection(this);
      cross_simple_seq2.start(cross_env2.cross_agent2.seqr2);
      phase.drop_objection(this);
   endtask : main_phase

endclass

`endif
