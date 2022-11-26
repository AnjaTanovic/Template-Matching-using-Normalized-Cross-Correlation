----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 06/07/2022 12:08:34 PM
-- Design Name: 
-- Module Name: tb_top - tb
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity tb_top is
    generic(
                tmpl_width_g:positive := 8;
                img_width_g:positive := 10;
                result_width_g:positive := 32;
                size_tmp_g:positive := 51000;
                size_img_g:positive := 160000;
                size_res_g:positive := 800;
                pixel_4Xwidth_g: positive := 32;
                pixel_width_g : positive := 8
            );
--  Port ( );
end tb_top;

architecture tb of tb_top is
    
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
            pixel_4Xwidth_g: positive := 32
            );
    Port ( clk : in std_logic;
           reset : in std_logic;
           start: in std_logic;
           reset_stripe : in std_logic;
           
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
           signal clk_s, reset_s, start_s, reset_stripe_s, ready_s: std_logic;
           signal tmpl_cols_s, tmpl_rows_s: std_logic_vector(tmpl_width_g - 1 downto 0);
           signal img_cols_s, sum2_addr_s : std_logic_vector(img_width_g -1 downto 0);
           signal sum1_addr_bram_ctrl_s, resp_addr_bram_ctrl_s : std_logic_vector(img_width_g +2 -1 downto 0);
           signal img_data_s, tmpl_data_s: std_logic_vector(pixel_4Xwidth_g -1 downto 0);
           signal img_addr_bram_ctrl_s: std_logic_vector(img_width_g + tmpl_width_g +2 -1 downto 0);
           signal tmpl_addr_bram_ctrl_s: std_logic_vector(tmpl_width_g + tmpl_width_g +2 -1 downto 0);
           signal sum1_data_s, sum2_data_s, resp_data_s: std_logic_vector(result_width_g - 1 downto 0);
           signal we_img_s, we_tmpl_s : std_logic_vector(3 downto 0);

           constant tmpl_cols_c: integer := 3;
           constant tmpl_rows_c: integer := 2; 
           constant img_cols_c: integer := 5;
           
            type mem_img_t is array (0 to img_cols_c * tmpl_rows_c - 1) of std_logic_vector(pixel_4Xwidth_g - 1 downto 0);
            type mem_tmpl_t is array (0 to tmpl_cols_c * tmpl_rows_c - 1) of std_logic_vector(pixel_4Xwidth_g - 1 downto 0);
            constant mem_img_stripe_c: mem_img_t :=
                                                    (
                                                    std_logic_vector(to_signed(7, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(10, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(-14, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(100, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(102, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(30, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(40, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(-50, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(100, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(17, pixel_4Xwidth_g))
                                                    );
                                                
            constant mem_tmpl_c: mem_tmpl_t :=
                                                    (
                                                    std_logic_vector(to_signed(10, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(-14, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(100, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(40, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(-50, pixel_4Xwidth_g)),
                                                    std_logic_vector(to_signed(100, pixel_4Xwidth_g))
                                                    );                                                    
begin

    top_c: top_structure
            generic map(
                        tmpl_width_g => tmpl_width_g,
                        img_width_g => img_width_g,
                        result_width_g => result_width_g,
                        size_tmp_g => size_tmp_g,
                        size_img_g => size_img_g,
                        size_res_g => size_res_g,
                        pixel_4Xwidth_g => pixel_4Xwidth_g,
                        pixel_width_g => pixel_width_g
                        )
            port map(
                     clk => clk_s,
                     reset => reset_s,
                     start => start_s,
                     reset_stripe => reset_stripe_s,
                     ready_o => ready_s,
                     tmpl_cols_i => tmpl_cols_s,
                     tmpl_rows_i => tmpl_rows_s,
                     img_cols_i => img_cols_s,
                     sum1_addr_bram_ctrl_i => sum1_addr_bram_ctrl_s,
                     --sum2_addr_i => sum2_addr_s,
                     resp_addr_bram_ctrl_i => resp_addr_bram_ctrl_s,
                     img_data_i => img_data_s,
                     tmpl_data_i => tmpl_data_s,
                     img_addr_bram_ctrl_i => img_addr_bram_ctrl_s,
                     tmpl_addr_bram_ctrl_i => tmpl_addr_bram_ctrl_s,
                     sum1_data_o => sum1_data_s,
                     sum2_data_o => sum2_data_s,
                     resp_data_o => resp_data_s,
                     we_img_i => we_img_s,
                     we_tmpl_i => we_tmpl_s
                    );  
    clk_gen:process is
    begin
        clk_s <= '0', '1' after 50ns;
        wait for 100ns; 
    end process;
     
    
    stim_gen:process is  
    begin
        reset_s <= '1', '0' after 150ns;
        start_s <= '0';
        reset_stripe_s <= '0';
        wait until falling_edge(clk_s);
        reset_stripe_s <= '1', '0' after 100ns;
        wait until falling_edge(clk_s);
        tmpl_cols_s <= std_logic_vector(to_unsigned(tmpl_cols_c, tmpl_width_g));
        tmpl_rows_s <= std_logic_vector(to_unsigned(tmpl_rows_c, tmpl_width_g));
        img_cols_s <= std_logic_vector(to_unsigned(img_cols_c, img_width_g));
        wait until falling_edge(clk_s);
       
        
        --writes into mem for img
        we_img_s <= "1111";
        for i in 0 to img_cols_c * tmpl_rows_c - 1 loop
            img_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(i*4,img_width_g + tmpl_width_g + 2));
            img_data_s <= mem_img_stripe_c(i);
            wait until falling_edge(clk_s);
            wait until falling_edge(clk_s);
        end loop;
        we_img_s <= "0000";
        --writes into mem for tmpl
        we_tmpl_s <= "1111";
        for j in 0 to tmpl_cols_c * tmpl_rows_c - 1 loop
            tmpl_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(j*4,tmpl_width_g + tmpl_width_g + 2));
            tmpl_data_s <= mem_tmpl_c(j);
            wait until falling_edge(clk_s);
            wait until falling_edge(clk_s);
        end loop;
       -- we_tmpl_s <= "0000";
        
        --starts calc
        start_s <= '1';
        wait until falling_edge(clk_s);
        
        start_s <= '0';
        wait until ready_s = '1';
        wait until falling_edge(clk_s);
        
        --reads results from mem
        for k in 0 to img_cols_c - tmpl_cols_c loop
            sum1_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(k * 4, img_width_g + 2));
            --sum2_data_s <= std_logic_vector(to_unsigned(0, img_width_g));
            resp_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(k * 4, img_width_g + 2));
            wait until falling_edge(clk_s);
        end loop;
        
        --NEXT STRIPE
        
        reset_stripe_s <= '1', '0' after 150ns;
  
                --writes into mem for img
        we_img_s <= "1111";
        for i in 0 to img_cols_c * tmpl_rows_c - 1 loop
            img_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(i*4,img_width_g + tmpl_width_g + 2));
            img_data_s <= mem_img_stripe_c(i);
            wait until falling_edge(clk_s);
            wait until falling_edge(clk_s);
        end loop;
        --we_img_s <= "0000";
        
        --starts calc
        start_s <= '1';
        wait until falling_edge(clk_s);
        
        start_s <= '0';
        wait until ready_s = '1';
        wait until falling_edge(clk_s);
        
        --reads results from mem
        for k in 0 to img_cols_c - tmpl_cols_c loop
            sum1_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(k * 4, img_width_g + 2));
            --sum2_data_s <= std_logic_vector(to_unsigned(0, img_width_g));
            resp_addr_bram_ctrl_s <= std_logic_vector(to_unsigned(k * 4, img_width_g + 2));
            wait until falling_edge(clk_s);
        end loop;
        wait;
    end process; 
                          
end tb;

