-uvmhome "/eda/cadence/2019-20/RHELx86/XCELIUM_19.03.013/tools/methodology/UVM/CDNS-1.2/" 
-uvm +UVM_TESTNAME=test_simple2 +UVM_VERBOSITY=UVM_MEDIUM
-sv +incdir+../verif
-sv +incdir+../verif/Agent
-sv +incdir+../verif/Sequences
-sv +incdir+../verif/Configurations
 
 ../rtl/utils_pkg.vhd
 ../rtl/dsp_loop_beh.vhd
 ../rtl/dsp2_loop_beh.vhd
 ../rtl/loop_structure.vhd
 ../rtl/address_generation_rtl.vhd
 ../rtl/memory_beh.vhd
 ../rtl/calculation_rtl.vhd
 ../rtl/top_structure.vhd
 ../rtl/CROSS_CORRELATION_v1_0_S00_AXI.vhd
 ../rtl/CROSS_CORRELATION_v1_0.vhd

-sv ../verif/Configurations/configurations2_pkg.sv
-sv ../verif/Agent/agent2_pkg.sv
-sv ../verif/Sequences/seq2_pkg.sv
-sv ../verif/test2_pkg.sv
-sv ../verif/cross_corr_if.sv

-sv ../verif/verif_top2.sv



#-LINEDEBUG
-access +rwc
-disable_sem2009
-nowarn "MEMODR"
-timescale 1ns/10ps

