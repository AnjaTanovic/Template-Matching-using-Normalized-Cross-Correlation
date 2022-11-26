`ifndef VERIF_TOP_SV
 `define VERIF_TOP_SV
 
 module verif_top;

   import uvm_pkg::*;     // import the UVM library
   `include "uvm_macros.svh" // Include the UVM macros

   import test_pkg::*;

   logic clk;
   logic rst;
    
   // interface
   cross_corr_if cross_corr_vif(clk, rst);

   // DUT, povezati portove naseg duta 
   CROSS_CORRELATION_v1_0 DUT(
                             .s00_axi_aclk         ( clk ),
                             .s00_axi_aresetn      ( rst ),
                             .sum1_data_o    ( cross_corr_vif.sum1_data_o ),
                             .resp_data_o    ( cross_corr_vif.resp_data_o ),
                             .img_data_i     ( cross_corr_vif.img_data_i),
                             .tmpl_data_i    ( cross_corr_vif.tmpl_data_i),
                             .img_addr_i     ( cross_corr_vif. img_addr_i),
                             .tmpl_addr_i    ( cross_corr_vif.tmpl_addr_i),
                             .sum1_addr_i    ( cross_corr_vif.sum1_addr_i),
                             .resp_addr_i    ( cross_corr_vif.resp_addr_i),
                             .we_img_i       ( cross_corr_vif.we_img_i),
                             .we_tmpl_i      ( cross_corr_vif.we_tmpl_i),
                             
                             .s00_axi_awaddr      ( cross_corr_vif.s00_axi_awaddr),
                             .s00_axi_awprot      ( cross_corr_vif.s00_axi_awprot),
                             .s00_axi_awvalid      ( cross_corr_vif.s00_axi_awvalid),
                             .s00_axi_awready      ( cross_corr_vif.s00_axi_awready),
                             .s00_axi_wdata      ( cross_corr_vif.s00_axi_wdata),
                             .s00_axi_wstrb      ( cross_corr_vif.s00_axi_wstrb),
                             .s00_axi_wvalid      ( cross_corr_vif.s00_axi_wvalid),
                             .s00_axi_wready     ( cross_corr_vif.s00_axi_wready),
                             .s00_axi_bresp      ( cross_corr_vif.s00_axi_bresp),
                             .s00_axi_bvalid      ( cross_corr_vif.s00_axi_bvalid),
                             .s00_axi_bready      ( cross_corr_vif.s00_axi_bready),
                             .s00_axi_araddr      ( cross_corr_vif.s00_axi_araddr),
                             .s00_axi_arprot      ( cross_corr_vif.s00_axi_arprot),
                             .s00_axi_arvalid      ( cross_corr_vif.s00_axi_arvalid),
                             .s00_axi_arready      ( cross_corr_vif.s00_axi_arready),
                             .s00_axi_rdata      ( cross_corr_vif.s00_axi_rdata),
                             .s00_axi_rresp      ( cross_corr_vif.s00_axi_rresp),
                             .s00_axi_rvalid      ( cross_corr_vif.s00_axi_rvalid),
                             .s00_axi_rready      ( cross_corr_vif.s00_axi_rready)
                             );
                           

   // run test
   initial begin      
      uvm_config_db#(virtual cross_corr_if)::set(null, "uvm_test_top.cross_env", "cross_corr_if", cross_corr_vif);
      run_test();
   end
    
   // clock and reset init.
   initial begin
      clk <= 0;
      rst <= 0;
      #50 rst <= 1;
	  //dodati potrebnu inicijalizaciju
   end

   // clock generation
   always #50 clk = ~clk;

endmodule : verif_top

`endif

