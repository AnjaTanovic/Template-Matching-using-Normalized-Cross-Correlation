`ifndef DRIVER_SV
`define DRIVER_SV

class driver extends uvm_driver#(seq_item);

   `uvm_component_utils(driver)
   
   int loop_ready = 0;
   
   virtual interface cross_corr_if vif;
   
   function new(string name = "driver", uvm_component parent = null);
      super.new(name,parent);
      if (!uvm_config_db#(virtual cross_corr_if)::get(this, "", "cross_corr_if", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set for: ",get_full_name(),".vif"})
   endfunction

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      
   endfunction : connect_phase

   task main_phase(uvm_phase phase);
   
      forever begin
		@(posedge vif.clk);	 
		if (vif.rst)
		begin
		seq_item_port.get_next_item(req);  //uzme jedan seq item 
		`uvm_info(get_type_name(), $sformatf("Driver sending...\n%s", req.sprint()), UVM_HIGH)

        seq_item_port.item_done();
        
        if (req.bram_axi_ctrl == 0) begin
            //BRAM
            vif.we_img_i = req.we_img_i;
            vif.we_tmpl_i = req.we_tmpl_i;
    
            vif.tmpl_addr_i = req.tmpl_addr_i;
            vif.img_addr_i = req.img_addr_i;
                    
            vif.img_data_i = req.img_data_i;
            vif.tmpl_data_i = req.tmpl_data_i;
            
            vif.resp_addr_i = req.resp_addr_i;
            vif.sum1_addr_i = req.sum1_addr_i;
            
            $display("Data sent to bram");
        end
        else begin
            //REGISTERS IN AXI CTRL
            vif.s00_axi_awaddr = req.s00_axi_awaddr;
            vif.s00_axi_wdata = req.s00_axi_wdata;
            vif.s00_axi_wstrb = 4'b1111;
            vif.s00_axi_awvalid = 1'b1;
            vif.s00_axi_wvalid = 1'b1;
            vif.s00_axi_bready = 1'b1;
            
            @(posedge vif.clk iff vif.s00_axi_awready);
            @(posedge vif.clk iff !vif.s00_axi_awready);
            #20
            vif.s00_axi_awvalid = 1'b0;
            vif.s00_axi_awaddr = 1'b0;
            vif.s00_axi_wdata = 1'b0;
            vif.s00_axi_wvalid = 1'b0;
            vif.s00_axi_wstrb = 4'b0000;
            
            @(posedge vif.clk iff !vif.s00_axi_bvalid);
            #20
            vif.s00_axi_bready = 1'b0;
            $display("Data sent to AXI Lite registers");
            
            //Waiting for ready
            if (req.s00_axi_awaddr == 0 && req.s00_axi_wdata == 0)
            begin
                loop_ready = 0;
                while (!loop_ready) begin
                    @(posedge vif.clk)
                    vif.s00_axi_araddr = 5'b10000; //ready register
                    vif.s00_axi_arvalid = 1'b1;
                    vif.s00_axi_rready = 1'b1;
                    
                    @(posedge vif.clk iff vif.s00_axi_arready);
                    @(posedge vif.clk iff !vif.s00_axi_arready);
                    #20
                    vif.s00_axi_araddr = 1'b0; 
                    vif.s00_axi_arvalid = 1'b0;
                    vif.s00_axi_rready = 1'b0;
                    
                    if (vif.s00_axi_rdata == 1)
                    begin
                        //ready = 1, exit loop
                        loop_ready = 1;
                        
                        @(posedge vif.clk);
                        //Read sum2
                        vif.s00_axi_araddr = 5'b10100; //sum2 register
                        vif.s00_axi_arvalid = 1'b1;
                        vif.s00_axi_rready = 1'b1;
                        
                        @(posedge vif.clk iff vif.s00_axi_arready);
                        #5
                        $display("Sum2 = %d", vif.s00_axi_rdata);
                        @(posedge vif.clk iff !vif.s00_axi_arready);
                        #20
                        vif.s00_axi_araddr = 1'b0;
                        vif.s00_axi_arvalid = 1'b0;
                        vif.s00_axi_rready = 1'b0;
                        
                        break;
                    end
                end
            end
          end  
       
		
		//`uvm_info(get_type_name(),$sformatf("Item done: %s\n",req.sprint()), UVM_HIGH)

		end
      end
   endtask : main_phase

endclass : driver

`endif


