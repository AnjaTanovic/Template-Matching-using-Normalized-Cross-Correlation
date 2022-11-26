`ifndef SEQ_ITEM_SV
`define SEQ_ITEM_SV

parameter tmpl_width_g = 8;
parameter img_width_g = 10;
parameter result_width_g = 32;
parameter pixel_width_g = 8;
parameter pixel_4Xwidth_g = 8;

parameter size_tmpl_g = 51000;
parameter size_img_g = 160000;
parameter size_res_g = 800;

parameter size_4Xtmpl_g = 204000;
parameter size_4Ximg_g = 640000;
parameter size_4Xres_g = 3200;

parameter C_S00_AXI_DATA_WIDTH = 32;
parameter C_S00_AXI_ADDR_WIDTH = 5;

class seq_item extends uvm_sequence_item;

    //control signals
    rand logic bram_axi_ctrl;  //0 - bram, 1 - axi
    rand logic [result_width_g -1:0] sum2_data; 
    rand int result_type;
    
    //interface signals
	rand logic [result_width_g -1:0] sum1_data_o;
	rand logic [result_width_g -1:0] resp_data_o;
	
    rand logic signed [pixel_4Xwidth_g -1:0] img_data_i;	
    rand logic signed [pixel_4Xwidth_g -1:0] tmpl_data_i;
	
	rand logic [img_width_g + tmpl_width_g +2 -1:0] img_addr_i;
	rand logic [tmpl_width_g + tmpl_width_g +2 -1:0] tmpl_addr_i;
	
	rand logic [img_width_g +2 -1:0] sum1_addr_i;
	rand logic [img_width_g +2 -1:0] resp_addr_i;

	rand logic [3:0] we_img_i;
	rand logic [3:0] we_tmpl_i;
       
//  Ports of Axi Slave Bus Interface S00_AXI

	rand logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_awaddr;
	rand logic [2:0] s00_axi_awprot;
	rand logic s00_axi_awvalid;
	rand logic s00_axi_awready;
	rand logic [C_S00_AXI_DATA_WIDTH -1:0] s00_axi_wdata;
	rand logic [(C_S00_AXI_DATA_WIDTH/8) -1:0] s00_axi_wstrb;
	rand logic s00_axi_wvalid;
	rand logic s00_axi_wready;
	rand logic [1:0] s00_axi_bresp;
	rand logic s00_axi_bvalid;
	rand logic s00_axi_bready;
	rand logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_araddr;
	rand logic [2:0] s00_axi_arprot;
	rand logic s00_axi_arvalid;
	rand logic s00_axi_arready;
	rand logic [C_S00_AXI_DATA_WIDTH - 1:0] s00_axi_rdata;
	rand logic [1:0] s00_axi_rresp;
	rand logic s00_axi_rvalid;
	rand logic s00_axi_rready;

	`uvm_object_utils_begin(seq_item)
	   
		`uvm_field_int(sum1_data_o, UVM_DEFAULT)
		`uvm_field_int(resp_data_o, UVM_DEFAULT)
		`uvm_field_int(img_data_i, UVM_DEFAULT)
		`uvm_field_int(tmpl_data_i, UVM_DEFAULT)
		`uvm_field_int(img_addr_i, UVM_DEFAULT)
		`uvm_field_int(tmpl_addr_i, UVM_DEFAULT)
		`uvm_field_int(sum1_addr_i, UVM_DEFAULT)
		`uvm_field_int(resp_addr_i, UVM_DEFAULT)
		`uvm_field_int(we_img_i, UVM_DEFAULT)
		`uvm_field_int(we_tmpl_i, UVM_DEFAULT)
		
		`uvm_field_int(s00_axi_awaddr, UVM_DEFAULT)
		`uvm_field_int(s00_axi_awprot, UVM_DEFAULT)
		`uvm_field_int(s00_axi_awvalid, UVM_DEFAULT)
		`uvm_field_int(s00_axi_awready, UVM_DEFAULT)
		`uvm_field_int(s00_axi_wdata, UVM_DEFAULT)
		`uvm_field_int(s00_axi_wstrb, UVM_DEFAULT)
		`uvm_field_int(s00_axi_wvalid, UVM_DEFAULT)
		`uvm_field_int(s00_axi_wready, UVM_DEFAULT)
		`uvm_field_int(s00_axi_bresp, UVM_DEFAULT)
		`uvm_field_int(s00_axi_bvalid, UVM_DEFAULT)
		`uvm_field_int(s00_axi_bready, UVM_DEFAULT)
		`uvm_field_int(s00_axi_araddr, UVM_DEFAULT)
		`uvm_field_int(s00_axi_arprot, UVM_DEFAULT)
		`uvm_field_int(s00_axi_arvalid, UVM_DEFAULT)
		`uvm_field_int(s00_axi_arready, UVM_DEFAULT)
		`uvm_field_int(s00_axi_rdata, UVM_DEFAULT)
		`uvm_field_int(s00_axi_rresp, UVM_DEFAULT)
		`uvm_field_int(s00_axi_rvalid, UVM_DEFAULT)
		`uvm_field_int(s00_axi_rready, UVM_DEFAULT)
   	`uvm_object_utils_end


    //Constraints for bram addresses
    constraint tmpl_mem_constr {tmpl_addr_i < size_4Xtmpl_g;}; 
    constraint img_mem_constr {img_addr_i < size_4Ximg_g;}; 
    constraint resp_mem_constr {resp_addr_i < size_4Xres_g;}; 
    constraint sum1_mem_constr {sum1_addr_i < size_4Xres_g;}; 
    
    //Constraints for enabling bytes for write axi lite
    constraint write_axi_lite {s00_axi_wstrb inside{4'b1111, 4'b0000};};
    
    //Constraints for enabling bytes for write bram
    constraint write_img_bram {we_img_i inside{4'b1111, 4'b0000};};
    constraint write_tmpl_bram {we_tmpl_i inside{4'b1111, 4'b0000};};
	
    function new (string name = "seq_item");
       super.new(name);
    endfunction // new

endclass : seq_item

`endif

