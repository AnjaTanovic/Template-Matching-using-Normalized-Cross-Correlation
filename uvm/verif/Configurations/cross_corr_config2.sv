parameter NUMBER_OF_VECTORS = 101;

class cross_corr_config2 extends uvm_object;
    
    uvm_active_passive_enum is_active = UVM_ACTIVE;
    
    rand int rand_const_init;
    int rand_const;
    
    int f_open;
    
    int tmpl_arr[$];
    int img_arr[$];
    int resp_arr[$];
    int sum1_arr[$];
    int sum2;
    
    int data;
    int i = 0;
    
    int j = 0;
    string num;
    
    int tmpl_cols;
    int tmpl_rows;
    int img_cols;
    int res_cols;
    
    logic j_parity;
    
    //Files
    string dimensions_files[NUMBER_OF_VECTORS];
    string img_files[NUMBER_OF_VECTORS];
    string tmpl_files[NUMBER_OF_VECTORS];
    string resp_files[NUMBER_OF_VECTORS];
    string sum1_files[NUMBER_OF_VECTORS];
    string sum2_files[NUMBER_OF_VECTORS];
    
    // covergroup
    covergroup stripe;
      option.per_instance = 1;
      rand_stripe : coverpoint rand_const {
        bins group1  = {[0:11]};
        bins group2  = {[12:21]};
        bins group3  = {[22:31]};
        bins group4  = {[32:41]};
        bins group5  = {[42:51]};
        bins group6  = {[52:61]};
        bins group7  = {[62:71]};
        bins group8  = {[72:81]};
        bins group9  = {[82:91]};
        bins group10  = {[92:101]};
      }
      parity: coverpoint j_parity;
    endgroup

    `uvm_object_utils_begin (cross_corr_config2)
      `uvm_field_enum(uvm_active_passive_enum, is_active, UVM_DEFAULT)
    `uvm_object_utils_end

    constraint rand_constr {rand_const_init >= 0;}; 
    
    function new(string name = "cross_corr_config2");
        super.new(name);
        
        // instance of covergroup
        stripe = new();
        
        //Files group 0
        dimensions_files[0] = "../golden_vectors/dimensions_file.txt";
        //string dimensions_file = " .../y22-g03/uvm/golden_vectors/dimensions_file.txt";
        img_files[0] = "../golden_vectors/img_file.txt";
        //string img_file = ".../y22-g03/uvm/golden_vectors/img_file.txt";
        tmpl_files[0] = "../golden_vectors/tmpl_file.txt";
        //string tmpl_file = ".../y22-g03/uvm/golden_vectors/tmpl_file.txt";
        resp_files[0] = "../golden_vectors/resp_file.txt";
        //string resp_file = ".../y22-g03/uvm/golden_vectors/resp_file.txt";
        sum1_files[0] = "../golden_vectors/sum1_file.txt";
        //string sum1_file = ".../y22-g03/uvm/golden_vectors/sum1_file.txt";
        sum2_files[0] = "../golden_vectors/sum2_file.txt";
        //string sum2_file = ".../y22-g03/uvm/golden_vectors/sum2_file.txt";
        
        //Files groups 1 - NUMBER_OF_VECTORS
        
        for (j = 1; j < NUMBER_OF_VECTORS; j++)
        begin
            num.itoa(j);
            dimensions_files[j] = {"../golden_vectors/dimensions_file_", num, ".txt"};
            img_files[j] = {"../golden_vectors/img_file_", num, ".txt"};
            tmpl_files[j] = {"../golden_vectors/tmpl_file_", num, ".txt"};
            resp_files[j] = {"../golden_vectors/resp_file_", num, ".txt"};
            sum1_files[j] = {"../golden_vectors/sum1_file_", num, ".txt"};
            sum2_files[j] = {"../golden_vectors/sum2_file_", num, ".txt"};
        end
    endfunction

    function randomize_config;
        $display("rand_const_init : %d", rand_const_init);    
        rand_const = (rand_const_init % NUMBER_OF_VECTORS);
        $display("rand_const : %d", rand_const);  
        
        //Dimensions
        f_open = $fopen(dimensions_files[rand_const], "r");  
        
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end
        
        $fscanf(f_open, "%d\n", tmpl_cols);
        $display("tmpl_cols: %d", tmpl_cols);
        $fscanf(f_open, "%d\n", tmpl_rows);
        $display("tmpl_rows: %d", tmpl_rows);
        $fscanf(f_open, "%d\n", img_cols);
        $display("img_cols: %d", img_cols);
        
        res_cols = img_cols - tmpl_cols;
        j_parity = res_cols % 2;
        
        //collect coverage (rand_stripe and parity)
        stripe.sample();
        
        $fclose(f_open);
        
        //Tmpl
        f_open = $fopen(tmpl_files[rand_const], "r");  
    
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end
       
        //$display("f_open je %d", f_open);
        
        i = 0;
        while ( !$feof(f_open)) begin
			$fscanf(f_open, "%d\n", data);
            $display("tmpl: (i, data) = %d, %d", i, data);
            
            tmpl_arr[i] = data;
            i = i + 1;
        end
        $display("Template stored to array successfully");
         
        $fclose(f_open);
        
        //Img
        f_open = $fopen(img_files[rand_const], "r");  
    
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end

        //$display("f_open je %d", f_open);
        
        i = 0;
        while (!$feof(f_open)) begin
            $fscanf(f_open, "%d\n",data);
            $display("img: (i, data) = %d, %d", i, data);
            
            img_arr[i] = data;
            i = i + 1;
        end
        $display("Image stripe stored to array successfully");
         
        $fclose(f_open);
        
        //Resp
        f_open = $fopen(resp_files[rand_const], "r");  
        
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end
        
        //$display("f_open je %d", f_open);
        
        i = 0;
        while ( !$feof(f_open)) begin
            $fscanf(f_open, "%d\n", data);
            $display("resp: (i, data) = %d, %d", i, data);
            
            resp_arr[i] = data;
            i = i + 1;
        end
        
        $display("Resp stored to array successfully");
        $fclose(f_open);
        
        //Sum1
		f_open = $fopen(sum1_files[rand_const], "r");  
    
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end
       
        //$display("f_open je %d", f_open);
        
        i = 0;
        while ( !$feof(f_open)) begin
			$fscanf(f_open, "%d\n", data);
            $display("sum1: (i, data) = %d, %d", i, data);
            
            sum1_arr[i] = data;
            i = i + 1;
        end
    
        $display("Sum1 stored to array successfully");
         
        $fclose(f_open);
        
        //Sum2
		f_open = $fopen(sum2_files[rand_const], "r");  
    
        if(f_open) begin
            $display("File was opened successfully");
        end	
        else begin
            $display("File was not opened successfully");
        end
       
        //$display("f_open je %d", f_open);
        
        while ( !$feof(f_open)) begin
			$fscanf(f_open, "%d\n", data);
            $display("sum2: %d", data);
            
            sum2 = data;
        end
    
        $display("Sum2 stored successfully");
         
        $fclose(f_open);
        
    endfunction : randomize_config
   
endclass : cross_corr_config2
