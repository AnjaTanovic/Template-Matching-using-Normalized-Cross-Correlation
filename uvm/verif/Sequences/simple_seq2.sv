`ifndef SIMPLE_SEQ2_SV
 `define SIMPLE_SEQ2_SV

class simple_seq2 extends base_seq2;

   `uvm_object_utils (simple_seq2)
      
   int i = 0;
   int j = 0;
   
   int data;
   int res_cols;
   int img_cols, tmpl_rows, tmpl_cols;
   
   covergroup img_data;
      option.per_instance = 1;
      data_value : coverpoint data {
        bins low_negative  = {[-128:-65]};
        bins high_negative  = {[-64:-1]};
        bins low_positive = {[0:63]};
        bins high_positive = {[64:127]};
      }
    endgroup
    
    covergroup tmpl_data;
      option.per_instance = 1;
      data_value : coverpoint data {
        bins low_negative  = {[-128:-65]};
        bins high_negative  = {[-64:-1]};
        bins low_positive = {[0:63]};
        bins high_positive = {[64:127]};
      }
    endgroup
    
   function new(string name = "simple_seq2");
      super.new(name);
      
      // instance of covergroup
      img_data = new();
      tmpl_data = new();
   endfunction

   virtual task body();
   
        //Load template to BRAM ////p_sequencer.cfg.tmpl_cols * p_sequencer.cfg.tmpl_rows
        res_cols = p_sequencer.cfg.res_cols + 1;
        img_cols = p_sequencer.cfg.img_cols;
        tmpl_cols = p_sequencer.cfg.tmpl_cols;
        tmpl_rows = p_sequencer.cfg.tmpl_rows;
        
        for (i = 0; i < p_sequencer.cfg.tmpl_arr.size; i++)
        begin
            j = i * 4;
            data = p_sequencer.cfg.tmpl_arr[i];
            tmpl_data.sample();
            `uvm_do_with(req, {req.tmpl_addr_i == j; req.we_tmpl_i == 4'b1111; req.we_img_i == 4'b0000; req.tmpl_data_i == data; req.s00_axi_awaddr != 0; req.bram_axi_ctrl == 0; req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.img_addr_i == 639996; req.s00_axi_araddr != 5'b10100;}) // resp and sum1 address hardcoded so they dont activate scoreboard
        end
      
         $display("Template stored to BRAM successfully");
         
        j = 0;
        
        //Load image stripe to BRAM
        for (i = 0; i < p_sequencer.cfg.img_arr.size; i++)
        begin
            j = i * 4;
            data = p_sequencer.cfg.img_arr[i];
            img_data.sample();
            `uvm_do_with(req, {req.img_addr_i == j; req.we_img_i == 4'b1111; req.we_tmpl_i == 4'b0000; req.img_data_i ==  data; req.s00_axi_awaddr != 0; req.bram_axi_ctrl == 0;  req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.tmpl_addr_i == 203996; req.s00_axi_araddr != 5'b10100;})
        end
       
         $display("Image stored to BRAM successfully");	  
        
        /****************************************************************/
        
        //Sending tmpl_cols
        `uvm_do_with(req, {req.s00_axi_awaddr == 4'b0100; req.s00_axi_wdata == tmpl_cols; req.bram_axi_ctrl == 1; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000;  req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.s00_axi_araddr != 5'b10100;})
        //Sending tmpl_rows
        `uvm_do_with(req, {req.s00_axi_awaddr == 4'b1000; req.s00_axi_wdata == tmpl_rows; req.bram_axi_ctrl == 1; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.s00_axi_araddr != 5'b10100;})
        //Sending img_cols
        `uvm_do_with(req, {req.s00_axi_awaddr == 4'b1100; req.s00_axi_wdata == img_cols; req.bram_axi_ctrl == 1; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.s00_axi_araddr != 5'b10100;})
        //Sending start bit
        `uvm_do_with(req, {req.s00_axi_awaddr == 4'b0000; req.s00_axi_wdata == 1; req.bram_axi_ctrl == 1; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.resp_addr_i == 4 * res_cols; req.sum1_addr_i == 4 * res_cols; req.s00_axi_araddr != 5'b10100;})
        //Disabling start bit
        `uvm_do_with(req, {req.s00_axi_awaddr == 4'b0000; req.s00_axi_wdata == 0; req.bram_axi_ctrl == 1; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.resp_addr_i == 4 *res_cols; req.sum1_addr_i == 4 *res_cols; req.s00_axi_araddr != 5'b10100;})
       
        /****************************************************************/
        
        //Reading results from BRAM
        
        //Resp
        for (i = 0; i < p_sequencer.cfg.resp_arr.size; i++)
        begin
            j = i * 4;
            `uvm_do_with(req, {req.resp_addr_i == j; req.sum1_addr_i == 4 * res_cols; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.s00_axi_awaddr != 0; req.bram_axi_ctrl == 0;})
        end
        //Sum1
        for (i = 0; i < p_sequencer.cfg.sum1_arr.size; i++)
        begin
            j = i * 4;
            `uvm_do_with(req, {req.sum1_addr_i == j; req.resp_addr_i == 4 * res_cols; req.we_img_i == 4'b0000; req.we_tmpl_i == 4'b0000; req.s00_axi_awaddr != 0; req.bram_axi_ctrl == 0;})
        end
        //Sum2 is sent in driver when ready appears
        
        $display("Result addresses sent");
        
	    #200;
	    
	    $display("Checking done\n\n");
   endtask : body

endclass : simple_seq2

`endif

