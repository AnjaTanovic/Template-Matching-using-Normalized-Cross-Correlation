`ifndef CROSS_CORR_IF_SV
`define CROSS_CORR_IF_SV

interface cross_corr_if (input clk, logic rst);
    
    
        parameter tmpl_width_g = 8;
        parameter img_width_g = 10;
        parameter result_width_g = 32;
        parameter size_tmpl_g = 51000;
        parameter size_img_g = 160000;
        parameter size_res_g = 800;
        parameter pixel_width_g = 8;
        parameter pixel_4Xwidth_g = 32;
        
        parameter	C_S00_AXI_DATA_WIDTH = 32;
        parameter	C_S00_AXI_ADDR_WIDTH =  5;
    
    
        logic [result_width_g - 1 : 0] sum1_data_o;
        logic [result_width_g - 1 : 0] resp_data_o;
        logic [pixel_4Xwidth_g -1 : 0] img_data_i;
        logic [pixel_4Xwidth_g -1: 0] tmpl_data_i;
        logic [img_width_g + tmpl_width_g +2 -1 : 0]  img_addr_i;
        logic [tmpl_width_g + tmpl_width_g  +2 -1 : 0] tmpl_addr_i;
        logic [img_width_g +2 -1 : 0] sum1_addr_i;
        logic [img_width_g +2 -1  : 0]  resp_addr_i;
        logic [3 : 0] we_img_i;
        logic [3 : 0]  we_tmpl_i ;
        
        logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_awaddr;
        logic [2:0] s00_axi_awprot;
        logic s00_axi_awvalid;
        logic s00_axi_awready;
        logic [C_S00_AXI_DATA_WIDTH -1:0] s00_axi_wdata;
        logic [(C_S00_AXI_DATA_WIDTH/8) -1:0] s00_axi_wstrb;
        logic s00_axi_wvalid;
        logic s00_axi_wready;
        logic [1:0] s00_axi_bresp;
        logic s00_axi_bvalid;
        logic s00_axi_bready;
        logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_araddr;
        logic [2:0] s00_axi_arprot;
        logic s00_axi_arvalid;
        logic s00_axi_arready;
        logic [C_S00_AXI_DATA_WIDTH - 1:0] s00_axi_rdata;
        logic [1:0] s00_axi_rresp;
        logic s00_axi_rvalid;
        logic s00_axi_rready;
	
endinterface : cross_corr_if

`endif

  

