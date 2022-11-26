# ===================================================================================
# Definisanje direktorijuma u kojem ce biti projekat
# ===================================================================================
cd ..
set root_dir [pwd]
cd scripts
set resultDir ../uvm_project

file mkdir $resultDir

create_project cross_correlation $resultDir -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]

# ===================================================================================
# Ukljucivanje svih izvornih i simulacionih fajlova u projekat
# ===================================================================================
add_files -norecurse ../rtl/utils_pkg.vhd
add_files -norecurse ../rtl/dsp_loop_beh.vhd
add_files -norecurse ../rtl/dsp2_loop_beh.vhd
add_files -norecurse ../rtl/loop_structure.vhd
add_files -norecurse ../rtl/memory_beh.vhd
add_files -norecurse ../rtl/calculation_rtl.vhd
add_files -norecurse ../rtl/address_generation_rtl.vhd
add_files -norecurse ../rtl/top_structure.vhd
add_files -norecurse ../rtl/CROSS_CORRELATION_v1_0_S00_AXI.vhd
add_files -norecurse ../rtl/CROSS_CORRELATION_v1_0.vhd

update_compile_order -fileset sources_1

set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/Agent/agent_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/Configurations/configurations_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/Sequences/seq_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/test_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/cross_corr_if.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/verif_top.sv

update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

# ===================================================================================
# Ukljucivanje uvm biblioteke
# ===================================================================================
set_property -name {xsim.compile.xvlog.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.elaborate.xelab.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.simulate.xsim.more_options} -value {-testplusarg UVM_TESTNAME=test_simple -testplusarg UVM_VERBOSITY=UVM_MEDIUM} -objects [get_filesets sim_1]