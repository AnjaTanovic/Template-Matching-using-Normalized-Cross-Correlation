`ifndef SCBD2
`define SCBD2

class scoreboard2 extends uvm_scoreboard;
	
	// control fileds
	bit checks_enable = 1;
	bit coverage_enable = 1;

    int f_open;
    
    cross_corr_config2 cfg;
    
	uvm_analysis_imp#(seq_item, scoreboard2) item_collected_import;

	int num_of_tr;
	
	`uvm_component_utils_begin(scoreboard2)
		`uvm_field_int(checks_enable, UVM_DEFAULT)
		`uvm_field_int(coverage_enable, UVM_DEFAULT)
	`uvm_component_utils_end

	function new(string name = "scoreboard2", uvm_component parent = null);
		super.new(name,parent);
		item_collected_import = new("item_collected_import", this);
		
		/************Geting from configuration database*******************/
        if(!uvm_config_db#(cross_corr_config2)::get(this, "", "cross_corr_config2", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
        /*****************************************************************/
	endfunction

	function void connect_phase(uvm_phase phase);
		super.connect_phase(phase);

	endfunction : connect_phase 

	function void write(seq_item curr_it);
	  
	 if(checks_enable) begin
        $display("Scoreboard function write called");


        if (curr_it.result_type == 1) // resp
		begin
			asrt_resp : assert (curr_it.resp_data_o == cfg.resp_arr[curr_it.resp_addr_i/4])
				`uvm_info(get_type_name(), "Resp check succesfull", UVM_MEDIUM)
			else
				`uvm_error(get_type_name(), $sformatf("Observed mismatch for resp[%d]. Observed value is %0d, expected is %0d.",curr_it.resp_addr_i/4, curr_it.resp_data_o, cfg.resp_arr[curr_it.resp_addr_i/4]))
		end
		
		if (curr_it.result_type == 2) // sum1
		begin
			asrt_sum1 : assert (curr_it.sum1_data_o == cfg.sum1_arr[curr_it.sum1_addr_i/4])
				`uvm_info(get_type_name(), "Sum1 check succesfull", UVM_MEDIUM)
			else
				`uvm_error(get_type_name(), $sformatf("Observed mismatch for sum1[%d]. Observed value is %0d, expected is %0d.",curr_it.sum1_addr_i/4, curr_it.sum1_data_o, cfg.sum1_arr[curr_it.sum1_addr_i/4]))
		end
		
		if (curr_it.result_type == 3) // sum2
		begin
			asrt_sum2 : assert (curr_it.sum2_data == cfg.sum2)
				`uvm_info(get_type_name(), "Sum2 check succesfull", UVM_MEDIUM)
			else
				`uvm_error(get_type_name(), $sformatf("Observed mismatch for sum2. Observed value is %0d, expected is %0d.", curr_it.sum2_data, cfg.sum2))
		end
		
		 ++num_of_tr;
	 end
	endfunction
	
	function void report_phase(uvm_phase phase);
      `uvm_info(get_type_name(), $sformatf("Scoreboard examined: %0d transactions", num_of_tr), UVM_LOW);
    endfunction : report_phase

endclass : scoreboard2
`endif

