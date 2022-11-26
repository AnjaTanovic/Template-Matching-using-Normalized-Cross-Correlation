# ===================================================================================
# Definisanje direktorijuma u kojima ce biti smesteni projekat i konfiguracioni fajl
# ===================================================================================
cd ..
set root_dir [pwd]
cd scripts

set ipDir ../result/cross_correlation_ip
set resultDir ../result/cross_correlation_design
set releaseDir ../release/cross_correlation

file mkdir $ipDir
file mkdir $resultDir
file mkdir $releaseDir

create_project cross_correlation_ip ../result/cross_correlation_ip -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]

# ===================================================================================
# Ukljucivanje svih izvornih fajlova u projekat
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

#add_files -fileset constrs_1 ../constraints/$clk_constraint.xdc

update_compile_order -fileset sources_1

# ===================================================================================
# Pakovanje IP jezgra
# ===================================================================================
ipx::package_project -root_dir $root_dir/result/ip_repo -vendor xilinx.com -library user -taxonomy /UserIP -import_files -set_current false
ipx::unload_core $root_dir/result/ip_repo/component.xml
ipx::edit_ip_in_project -upgrade true -name cross_corr_ip -directory $root_dir/result/ip_repo $root_dir/result/ip_repo/component.xml
update_compile_order -fileset sources_1

set_property vendor FTN [ipx::current_core]
set_property name CROSS_CORRELATION [ipx::current_core]
set_property display_name CROSS_CORRELATION_v1_0 [ipx::current_core]
set_property description {Module for cross correlation operation} [ipx::current_core]
set_property vendor_display_name FTN [ipx::current_core]
set_property supported_families {zynq Production} [ipx::current_core]

set_property core_revision 2 [ipx::current_core]
ipx::update_source_project_archive -component [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property  ip_repo_paths  $root_dir/result/ip_repo [current_project]
update_ip_catalog
ipx::check_integrity -quiet [ipx::current_core]
close_project

update_compile_order -fileset sources_1
close_project

# ===================================================================================
# Kreiranje block dizajna
# ===================================================================================
create_project cross_correlation_design ../result/cross_correlation_design -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]
set_property  ip_repo_paths  $root_dir/result/ip_repo [current_project]
update_ip_catalog
create_bd_design cross_correlation
update_compile_order -fileset sources_1
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]
set_property -dict [list CONFIG.PCW_QSPI_PERIPHERAL_ENABLE {1} CONFIG.PCW_ENET0_PERIPHERAL_ENABLE {1} CONFIG.PCW_SD0_PERIPHERAL_ENABLE {1} CONFIG.PCW_UART1_PERIPHERAL_ENABLE {1} CONFIG.PCW_SPI0_PERIPHERAL_ENABLE {1} CONFIG.PCW_USB0_PERIPHERAL_ENABLE {1} CONFIG.PCW_I2C0_PERIPHERAL_ENABLE {1} CONFIG.PCW_GPIO_MIO_GPIO_ENABLE {1}] [get_bd_cells processing_system7_0]
set_property -dict [list CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {114}] [get_bd_cells processing_system7_0]
startgroup
create_bd_cell -type ip -vlnv FTN:user:CROSS_CORRELATION:1.0 CROSS_CORRELATION_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_1
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_2
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_3
endgroup
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_3]
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_2]
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_1]
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_0]
startgroup
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_0/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_1/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_1/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_2/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_2/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_3/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_3/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/CROSS_CORRELATION_0/s00_axi} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins CROSS_CORRELATION_0/s00_axi]
endgroup
set_property range 1M [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_0_Mem0}]
set_property range 256K [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_1_Mem0}]
set_property range 4K [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_2_Mem0}]
set_property range 4K [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_3_Mem0}]
validate_bd_design
connect_bd_net [get_bd_pins axi_bram_ctrl_0/bram_addr_a] [get_bd_pins CROSS_CORRELATION_0/img_addr_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_0/bram_wrdata_a] [get_bd_pins CROSS_CORRELATION_0/img_data_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_0/bram_we_a] [get_bd_pins CROSS_CORRELATION_0/we_img_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_1/bram_addr_a] [get_bd_pins CROSS_CORRELATION_0/tmpl_addr_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_1/bram_wrdata_a] [get_bd_pins CROSS_CORRELATION_0/tmpl_data_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_1/bram_we_a] [get_bd_pins CROSS_CORRELATION_0/we_tmpl_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_2/bram_addr_a] [get_bd_pins CROSS_CORRELATION_0/resp_addr_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_2/bram_rddata_a] [get_bd_pins CROSS_CORRELATION_0/resp_data_o]
connect_bd_net [get_bd_pins axi_bram_ctrl_3/bram_addr_a] [get_bd_pins CROSS_CORRELATION_0/sum1_addr_i]
connect_bd_net [get_bd_pins axi_bram_ctrl_3/bram_rddata_a] [get_bd_pins CROSS_CORRELATION_0/sum1_data_o]
validate_bd_design

regenerate_bd_layout
assign_bd_address
save_bd_design

# ===================================================================================
# Kreiranje wrappera
# ===================================================================================
make_wrapper -files [get_files $resultDir/cross_correlation_design.srcs/sources_1/bd/cross_correlation/cross_correlation.bd] -top
add_files -norecurse $resultDir/cross_correlation_design.gen/sources_1/bd/cross_correlation/hdl/cross_correlation_wrapper.v

# ===================================================================================
# Pokretanje procesa sinteze
# ===================================================================================
launch_runs synth_1 -jobs 6
wait_on_run synth_1

puts "*****************************************************"
puts "* Sinteza zavrsena! *"
puts "*****************************************************"

# ===================================================================================
# Pokretanje procesa implementacije i generisanja konfiguracionog fajla
# ===================================================================================
launch_runs impl_1 -jobs 6
wait_on_run impl_1
set_property STEPS.WRITE_BITSTREAM.TCL.PRE [pwd]/pre_write_bitstream.tcl [get_runs impl_1]
launch_runs impl_1 -to_step write_bitstream
wait_on_run impl_1

puts "*******************************************************"
puts "* Implementacija zavrsena! *"
puts "*******************************************************"

# ===================================================================================
# Kopiranje konfiguracionog fajla (bitstream) u release folder
# ===================================================================================
update_compile_order -fileset sources_1
file copy -force $resultDir/cross_correlation_design.runs/impl_1/cross_correlation_wrapper.bit $releaseDir/cross_correlation_wrapper.bit

# ===================================================================================
# Eksportovanje hardvera
# ===================================================================================
write_hw_platform -fixed -include_bit -force -file $resultDir/cross_correlation_wrapper.xsa

# ===================================================================================
# Kopiranje .xsa fajla u folder vitis
# ===================================================================================
file copy -force $resultDir/cross_correlation_wrapper.xsa $root_dir/vitis/cross_correlation_wrapper.xsa


