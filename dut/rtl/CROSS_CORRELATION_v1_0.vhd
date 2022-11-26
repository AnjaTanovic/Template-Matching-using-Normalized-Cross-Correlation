library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity CROSS_CORRELATION_v1_0 is
	generic (
		-- Users to add parameters here
        tmpl_width_g:positive := 8;
        img_width_g:positive := 10;
        result_width_g:positive := 32;
        size_tmp_g:positive := 51000;
        size_img_g:positive := 160000;
        --size_img_g:positive := 132000;
        size_res_g:positive := 800;
        pixel_width_g : positive := 8;
        pixel_4Xwidth_g : positive := 32; 
		-- User parameters ends
		-- Do not modify the parameters beyond this line


		-- Parameters of Axi Slave Bus Interface S00_AXI
		C_S00_AXI_DATA_WIDTH	: integer	:= 32;
		C_S00_AXI_ADDR_WIDTH	: integer	:= 5
	);
	port (
		-- Users to add ports here
        sum1_data_o: out std_logic_vector(result_width_g - 1 downto 0);
        --sum2_data_o: out std_logic_vector(result_width_g - 1 downto 0);
        resp_data_o: out std_logic_vector(result_width_g - 1 downto 0);

        img_data_i : in std_logic_vector(pixel_4Xwidth_g -1 downto 0);
        tmpl_data_i : in std_logic_vector(pixel_4Xwidth_g -1 downto 0);
       
        img_addr_i : in std_logic_vector(img_width_g + tmpl_width_g +2 -1 downto 0);
        tmpl_addr_i : in std_logic_vector(tmpl_width_g + tmpl_width_g +2 -1 downto 0);
       
        sum1_addr_i : in std_logic_vector(img_width_g +2 -1 downto 0); 
        resp_addr_i : in std_logic_vector(img_width_g +2 -1 downto 0);
        
        we_img_i : in std_logic_vector(3 downto 0);
        we_tmpl_i : in std_logic_vector(3 downto 0);
		-- User ports ends
		-- Do not modify the ports beyond this line


		-- Ports of Axi Slave Bus Interface S00_AXI
		s00_axi_aclk	: in std_logic;
		s00_axi_aresetn	: in std_logic;
		s00_axi_awaddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_awprot	: in std_logic_vector(2 downto 0);
		s00_axi_awvalid	: in std_logic;
		s00_axi_awready	: out std_logic;
		s00_axi_wdata	: in std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_wstrb	: in std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
		s00_axi_wvalid	: in std_logic;
		s00_axi_wready	: out std_logic;
		s00_axi_bresp	: out std_logic_vector(1 downto 0);
		s00_axi_bvalid	: out std_logic;
		s00_axi_bready	: in std_logic;
		s00_axi_araddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_arprot	: in std_logic_vector(2 downto 0);
		s00_axi_arvalid	: in std_logic;
		s00_axi_arready	: out std_logic;
		s00_axi_rdata	: out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_rresp	: out std_logic_vector(1 downto 0);
		s00_axi_rvalid	: out std_logic;
		s00_axi_rready	: in std_logic
	);
end CROSS_CORRELATION_v1_0;

architecture arch_imp of CROSS_CORRELATION_v1_0 is
    -- instance top_stucture component
    
    component top_structure
    generic(
            tmpl_width_g:positive := 8;
            img_width_g:positive := 10;
            result_width_g:positive := 32;
            size_tmp_g:positive := 51000;
            size_img_g:positive := 160000;
            --size_img_g:positive := 132000;
            size_res_g:positive := 800;
            pixel_width_g : positive := 8;
            pixel_4Xwidth_g : positive := 32
            );
    Port ( clk : in std_logic;
           reset : in std_logic;
           start: in std_logic;
           reset_stripe: in std_logic;
           
           tmpl_cols_i : in std_logic_vector(tmpl_width_g - 1 downto 0);
           tmpl_rows_i : in std_logic_vector(tmpl_width_g - 1 downto 0);
           img_cols_i : in std_logic_vector(img_width_g - 1 downto 0);
           
           ready_o : out std_logic;
           
           -- input/output for communication with bram_ctrl 
           sum1_data_o: out std_logic_vector(result_width_g - 1 downto 0);
           sum2_data_o: out std_logic_vector(result_width_g - 1 downto 0);
           resp_data_o: out std_logic_vector(result_width_g - 1 downto 0);

           img_data_i : in std_logic_vector(pixel_4Xwidth_g -1 downto 0);
           tmpl_data_i : in std_logic_vector(pixel_4Xwidth_g -1 downto 0);
           
           img_addr_bram_ctrl_i : in std_logic_vector(img_width_g + tmpl_width_g +2 -1 downto 0);
           tmpl_addr_bram_ctrl_i : in std_logic_vector(tmpl_width_g + tmpl_width_g +2  -1 downto 0);
           
           sum1_addr_bram_ctrl_i : in std_logic_vector(img_width_g + 2 -1 downto 0);
           --sum2_addr_i : in std_logic_vector(img_width_g -1 downto 0);
           resp_addr_bram_ctrl_i : in std_logic_vector(img_width_g +2 -1 downto 0);
           
           we_img_i : in std_logic_vector(3 downto 0);
           we_tmpl_i : in std_logic_vector(3 downto 0)
           --bram_ctrl_sel_i : in std_logic 
           ); 
    end component;
    
	-- component declaration
	component CROSS_CORRELATION_v1_0_S00_AXI 
		generic (
		tmpl_width_g:positive := 8;
        img_width_g:positive := 10;
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 5
		);
		port (
		start_axi_o : out std_logic;
        tmpl_cols_axi_o : out std_logic_vector(tmpl_width_g - 1 downto 0);
        tmpl_rows_axi_o : out std_logic_vector(tmpl_width_g - 1 downto 0);
        img_cols_axi_o : out std_logic_vector(img_width_g - 1 downto 0);
        sum2_data_axi_i : in std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
        ready_axi_i : in std_logic;
        reset_stripe_axi_o: out std_logic;
		S_AXI_ACLK	: in std_logic;
		S_AXI_ARESETN	: in std_logic;
		S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
		S_AXI_AWVALID	: in std_logic;
		S_AXI_AWREADY	: out std_logic;
		S_AXI_WDATA	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_WSTRB	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		S_AXI_WVALID	: in std_logic;
		S_AXI_WREADY	: out std_logic;
		S_AXI_BRESP	: out std_logic_vector(1 downto 0);
		S_AXI_BVALID	: out std_logic;
		S_AXI_BREADY	: in std_logic;
		S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
		S_AXI_ARVALID	: in std_logic;
		S_AXI_ARREADY	: out std_logic;
		S_AXI_RDATA	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_RRESP	: out std_logic_vector(1 downto 0);
		S_AXI_RVALID	: out std_logic;
		S_AXI_RREADY	: in std_logic
		);
	end component;

      signal  start_axi_s : std_logic;
      signal  tmpl_cols_axi_s :  std_logic_vector(tmpl_width_g - 1 downto 0);
      signal  tmpl_rows_axi_s :  std_logic_vector(tmpl_width_g - 1 downto 0);
      signal  img_cols_axi_s : std_logic_vector(img_width_g - 1 downto 0);
      signal  sum2_data_axi_s :  std_logic_vector(C_S00_AXI_DATA_WIDTH - 1 downto 0);
      signal  ready_axi_s :  std_logic;
      signal  reset_stripe_axi_s :  std_logic;
      signal sum1_data_s, resp_data_s : std_logic_vector(result_width_g -1 downto 0); 
      signal img_data_s, tmpl_data_s : std_logic_vector(pixel_4Xwidth_g -1 downto 0);
      signal img_addr_s : std_logic_vector(img_width_g + tmpl_width_g + 2 -1 downto 0);
      signal tmpl_addr_s : std_logic_vector(tmpl_width_g + tmpl_width_g + 2 -1 downto 0);
      signal sum1_addr_s , resp_addr_s: std_logic_vector(img_width_g +2 -1 downto 0);
      signal we_img_s, we_tmpl_s : std_logic_vector(3 downto 0);
      
      signal top_structure_reset : std_logic;
begin

top_structure_reset <= not s00_axi_aresetn;

-- Instantiation of Axi Bus Interface S00_AXI
CROSS_CORRELATION_v1_0_S00_AXI_inst : CROSS_CORRELATION_v1_0_S00_AXI
	generic map (
	    tmpl_width_g => tmpl_width_g,
	    img_width_g => img_width_g,
		C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
		C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	)
	port map (
	    start_axi_o => start_axi_s,
		tmpl_cols_axi_o => tmpl_cols_axi_s,
		tmpl_rows_axi_o => tmpl_rows_axi_s,
		img_cols_axi_o => img_cols_axi_s,
		sum2_data_axi_i => sum2_data_axi_s,
		ready_axi_i => ready_axi_s,
		reset_stripe_axi_o => reset_stripe_axi_s,
		S_AXI_ACLK	=> s00_axi_aclk,
		S_AXI_ARESETN	=> s00_axi_aresetn,
		S_AXI_AWADDR	=> s00_axi_awaddr,
		S_AXI_AWPROT	=> s00_axi_awprot,
		S_AXI_AWVALID	=> s00_axi_awvalid,
		S_AXI_AWREADY	=> s00_axi_awready,
		S_AXI_WDATA	=> s00_axi_wdata,
		S_AXI_WSTRB	=> s00_axi_wstrb,
		S_AXI_WVALID	=> s00_axi_wvalid,
		S_AXI_WREADY	=> s00_axi_wready,
		S_AXI_BRESP	=> s00_axi_bresp,
		S_AXI_BVALID	=> s00_axi_bvalid,
		S_AXI_BREADY	=> s00_axi_bready,
		S_AXI_ARADDR	=> s00_axi_araddr,
		S_AXI_ARPROT	=> s00_axi_arprot,
		S_AXI_ARVALID	=> s00_axi_arvalid,
		S_AXI_ARREADY	=> s00_axi_arready,
		S_AXI_RDATA	=> s00_axi_rdata,
		S_AXI_RRESP	=> s00_axi_rresp,
		S_AXI_RVALID	=> s00_axi_rvalid,
		S_AXI_RREADY	=> s00_axi_rready
	);

	-- Add user logic here
    top_ins: top_structure
    generic map (
	    tmpl_width_g => tmpl_width_g,
	    img_width_g => img_width_g,
		result_width_g => 32,
        size_tmp_g => 51000,
        size_img_g => 160000,
        size_res_g => 800,
        pixel_width_g => 8,
        pixel_4Xwidth_g => 32
	)
	port map (
       clk => s00_axi_aclk,
       reset => top_structure_reset,
       start => start_axi_s,
       reset_stripe => reset_stripe_axi_s,
       tmpl_cols_i => tmpl_cols_axi_s,
       tmpl_rows_i => tmpl_rows_axi_s,
       img_cols_i => img_cols_axi_s,
       ready_o => ready_axi_s,
       
       -- input/output for communication with bram_ctrl 
       
       sum1_data_o => sum1_data_s,
       sum2_data_o => sum2_data_axi_s,
       resp_data_o => resp_data_s,
       img_data_i => img_data_s,
       tmpl_data_i => tmpl_data_s,
       img_addr_bram_ctrl_i => img_addr_s,
       tmpl_addr_bram_ctrl_i => tmpl_addr_s,
       sum1_addr_bram_ctrl_i => sum1_addr_s,
       resp_addr_bram_ctrl_i => resp_addr_s,
       we_img_i => we_img_s,
       we_tmpl_i => we_tmpl_s
	);
	-- connecting top with pin crossCorrelation IP block 
	
	sum1_data_o <= sum1_data_s;
	resp_data_o <= resp_data_s;
    img_data_s <= img_data_i;
    tmpl_data_s <= tmpl_data_i;
    img_addr_s <= img_addr_i;
    tmpl_addr_s <= tmpl_addr_i;
    sum1_addr_s <= sum1_addr_i;
    resp_addr_s <= resp_addr_i;
    we_img_s <= we_img_i;
    we_tmpl_s <= we_tmpl_i;
    
-- User logic ends
    
    
end arch_imp;

