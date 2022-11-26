class monitor2 extends uvm_monitor;

	// control fileds
	bit checks_enable = 1;
	bit coverage_enable = 1;
	   
	int x = 1;
	int resp_addr_i_1, resp_addr_i_2;
	int sum1_addr_i_1, sum1_addr_i_2;
   
   cross_corr_config2 cfg;

   uvm_analysis_port #(seq_item) item_collected_port;

   `uvm_component_utils_begin(monitor2)
      `uvm_field_int(checks_enable, UVM_DEFAULT)
      `uvm_field_int(coverage_enable, UVM_DEFAULT)
   `uvm_component_utils_end

   // The virtual interface used to drive and view HDL signals.
   virtual interface cross_corr_if vif;

   // current transaction
   seq_item curr_it;
   seq_item curr_it1;
   seq_item curr_it2;
   //seq_item curr_it_3;
   //seq_item curr_it_4;

   covergroup axi_registers;
      option.per_instance = 1;
      read_registers : coverpoint vif.s00_axi_awaddr {
        bins start  = {4'b0000};
        bins tmpl_cols  = {4'b0100};
        bins tmpl_rows = {4'b1000};
        bins img_cols = {4'b1100};
      }
      write_registers : coverpoint vif.s00_axi_araddr {
        bins ready  = {5'b10000};
        bins sum2  = {5'b10100};
      }
    endgroup

   function new(string name = "monitor2", uvm_component parent = null);
        super.new(name,parent);  
        
        // instance of covergroup
        axi_registers = new();
          
        item_collected_port = new("item_collected_port", this);
        if (!uvm_config_db#(virtual cross_corr_if)::get(this, "", "cross_corr_if", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
        
        /************Geting from configuration database*******************/
        if(!uvm_config_db#(cross_corr_config2)::get(this, "", "cross_corr_config2", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
        /*****************************************************************/
   endfunction

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      
   endfunction : connect_phase

   task main_phase(uvm_phase phase);
      forever begin
        		
		@(posedge vif.clk); //svaki clk proverava
        
        if (vif.rst)
        begin
        
            axi_registers.sample();
            
			curr_it = seq_item::type_id::create("curr_it", this);
			curr_it1 = seq_item::type_id::create("curr_it1", this);
			curr_it2 = seq_item::type_id::create("curr_it2", this);

            if (vif.resp_addr_i >= 0 && vif.resp_addr_i <= (4 * (cfg.img_cols - cfg.tmpl_cols)) && vif.resp_addr_i % 4 == 0) begin
                fork
		            begin
						if (x == 1) begin
							x = 0;
						end
						else begin
							x = 1;
						end

						if (x == 0) begin
				            //In scbd reading resp or sum1
				            resp_addr_i_1 = vif.resp_addr_i;
							//$display("addr 1 %d", resp_addr_i_1);
						end 
						else begin
							//In scbd reading resp or sum1
		                	resp_addr_i_2 = vif.resp_addr_i;
							//$display("addr 2 %d", resp_addr_i_2);
						end

		                //Wait for data on outputs
		                @(posedge vif.clk);

		                 if (x == 1) begin
		                    curr_it1.resp_data_o = vif.resp_data_o;
							curr_it1.result_type = 1;
							curr_it1.resp_addr_i = resp_addr_i_1;
							//$display("curr 1");
							item_collected_port.write(curr_it1);
							
						end
		                else begin
		                    curr_it2.resp_data_o = vif.resp_data_o;
							curr_it2.result_type = 1;
							curr_it2.resp_addr_i = resp_addr_i_2;
							//$display("curr 2");
							item_collected_port.write(curr_it2);
							
		               	end
		                //$display("Write called for resp or sum1");
		                //$display("result_type = %d", result_check);
		            end
            	join_none
            end
            if (vif.sum1_addr_i >= 0 && vif.sum1_addr_i <= (4 * (cfg.img_cols - cfg.tmpl_cols)) && vif.sum1_addr_i % 4 == 0) begin
            	fork
		            begin
						if (x == 1) begin
							x = 0;
						end
						else begin
							x = 1;
						end

						if (x == 0) begin
				            //In scbd reading resp or sum1
				            sum1_addr_i_1 = vif.sum1_addr_i;
							//$display("addr 1 %d", sum1_addr_i_1);
						end 
						else begin
							//In scbd reading resp or sum1
		                	sum1_addr_i_2 = vif.sum1_addr_i;
							//$display("addr 2 %d", sum1_addr_i_2);
						end

		                //Wait for data on outputs
		                @(posedge vif.clk);

		                 if (x == 1) begin
		                    curr_it1.sum1_data_o = vif.sum1_data_o;
							curr_it1.result_type = 2;
							curr_it1.sum1_addr_i = sum1_addr_i_1;
							//$display("curr 1");
							item_collected_port.write(curr_it1);
							
						end
		                else begin
		                    curr_it2.sum1_data_o = vif.sum1_data_o;
							curr_it2.result_type = 2;
							curr_it2.sum1_addr_i = sum1_addr_i_2;
							//$display("curr 2");
							item_collected_port.write(curr_it2);
							
		               	end
		                //$display("Write called for resp or sum1");
		                //$display("result_type = %d", result_check);
		            end
            	join_none    

            end
			
			if (vif.s00_axi_araddr == 5'b10100) //sum2
            begin
            	fork 
		            begin
		                
		                @(posedge vif.clk iff vif.s00_axi_arready);
		                #5
		                curr_it.sum2_data = vif.s00_axi_rdata;
		                curr_it.result_type = 3;
		                
		                item_collected_port.write(curr_it);
		                
		                @(posedge vif.clk);
		            end
                join_none
            end
            
        end

      end
   endtask : main_phase

endclass : monitor2

