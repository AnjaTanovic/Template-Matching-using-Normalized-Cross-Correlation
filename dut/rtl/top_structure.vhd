----------------------------------------------------------------------------------
-- File : top_structure.vhd 
-- Project: Template matching
-- Create Date: 05/31/2022 
-- Target Devices: Zybo Z7000
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
use work.utils_pkg.all;


-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top_structure is
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
end top_structure;

architecture structure of top_structure is

attribute use_dsp : string;
attribute use_dsp of structure : architecture is "yes";

component loop_structure 
    generic(
            tmpl_width_g:positive:=8;
            img_width_g:positive:=10
            );
    Port( 
        clk : in std_logic;
        reset : in std_logic;
        en : in std_logic;
    
        tmpl_cols_i : in std_logic_vector(tmpl_width_g -1 downto 0);
        tmpl_rows_i : in std_logic_vector(tmpl_width_g -1 downto 0);
        img_cols_i : in std_logic_vector(img_width_g -1 downto 0);
    
        m_o : out std_logic_vector(tmpl_width_g -1 downto 0);
        k_o : out std_logic_vector(tmpl_width_g -1 downto 0);
        j_even_o : out std_logic_vector(img_width_g -1 downto 0);
        j_odd_o : out std_logic_vector(img_width_g -1 downto 0);
    
        stripe_done_o : out std_logic;
        pixel_done_o : out std_logic
        );
end component;

component address_generation
    Generic (tmpl_width_g : positive := 8;
             img_width_g : positive := 10); 
           --address of tmpl: tmpl_width_g * tmpl_width_g
           --address of img: stripe img_width_g * tmpl_width_g
           --address of resp, sum1 and sum2: img_width_g 
    Port ( clk : in std_logic;   --komentarisati svaki port
           reset : in std_logic;
           en : in std_logic;
           tmpl_cols : in std_logic_vector(tmpl_width_g - 1 downto 0);
           img_cols : in std_logic_vector(img_width_g - 1 downto 0);
           m : in std_logic_vector(tmpl_width_g - 1 downto 0);
           k : in std_logic_vector(tmpl_width_g - 1 downto 0);
           j_even : in std_logic_vector(img_width_g - 1 downto 0);
           j_odd : in std_logic_vector(img_width_g - 1 downto 0);
           tmpl_addr : out std_logic_vector(tmpl_width_g + tmpl_width_g - 1 downto 0);
           img_even_addr : out std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
           img_odd_addr : out std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
           sum1_even_addr : out std_logic_vector(img_width_g - 1 downto 0);
           sum1_odd_addr : out std_logic_vector(img_width_g - 1 downto 0);
           --sum2_addr : out std_logic_vector(img_width_g - 1 downto 0);
           resp_even_addr : out std_logic_vector(img_width_g - 1 downto 0);
           resp_odd_addr : out std_logic_vector(img_width_g - 1 downto 0)
           );
   
    end component;
    
component calculation
    Generic (result_width_g : positive := 32;
             pixel_width_g : positive := 8); 
    Port ( clk : in std_logic;
           reset : in std_logic;
           en : in std_logic;
           img_pixel_even_data : in std_logic_vector(pixel_width_g - 1 downto 0);
           img_pixel_odd_data : in std_logic_vector(pixel_width_g - 1 downto 0);
           tmpl_pixel_data : in std_logic_vector(pixel_width_g - 1 downto 0);
           pixel_done : in std_logic;
           resp_even_data : out std_logic_vector(result_width_g - 1 downto 0);
           resp_odd_data : out std_logic_vector(result_width_g - 1 downto 0);
           sum1_even_data : out std_logic_vector(result_width_g - 1 downto 0);
           sum1_odd_data : out std_logic_vector(result_width_g - 1 downto 0);
           sum2_data : out std_logic_vector(result_width_g - 1 downto 0));
end component;

component memory_beh
    generic(
            width_g:positive:=8;
            size_g:positive:=51000
            );
    port(
        clka : in std_logic;
        clkb : in std_logic;
        ena: in std_logic;
        enb: in std_logic;
        wea: in std_logic_vector(3 downto 0);
        web: in std_logic_vector(3 downto 0);
        addra : in std_logic_vector(log2c(size_g)-1 downto 0);
        addrb : in std_logic_vector(log2c(size_g)-1 downto 0);
        dia: in std_logic_vector(width_g-1 downto 0);
        dib: in std_logic_vector(width_g-1 downto 0);
        doa: out std_logic_vector(width_g-1 downto 0);
        dob: out std_logic_vector(width_g-1 downto 0)
        );
end component;
-- signal for sum2 register
signal sum2_data_reg, sum2_data_next : std_logic_vector(result_width_g - 1 downto 0);

-- signals for loop_structure
signal m_s,k_s: std_logic_vector(tmpl_width_g -1 downto 0);
signal j_even_s, j_odd_s: std_logic_vector(img_width_g -1 downto 0);
signal stripe_done_s, pixel_done_s, en_reg, en_next: std_logic;

-- signals for addresses
signal tmpl_addr_s: std_logic_vector(tmpl_width_g + tmpl_width_g - 1 downto 0);
signal img_even_addr_s, img_odd_addr_s: std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
signal sum1_even_addr_s, sum1_odd_addr_s, resp_even_addr_s, resp_odd_addr_s, sum2_addr_s: std_logic_vector(img_width_g - 1 downto 0);

--signals for memory for tmpl
signal tmpl_pixel_data_s: std_logic_vector(pixel_width_g - 1 downto 0);

--signals for memory for img
signal img_pixel_even_data_s, img_pixel_odd_data_s: std_logic_vector(pixel_width_g - 1 downto 0);
signal img_addr_mux_s : std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);

--signal for memory for res
signal resp_even_data_s, resp_odd_data_s: std_logic_vector(result_width_g - 1 downto 0);
signal resp_addr_mux_s : std_logic_vector(img_width_g - 1 downto 0);

--signals for memory for sum1
signal sum1_even_data_s, sum1_odd_data_s: std_logic_vector(result_width_g - 1 downto 0);
signal sum1_addr_mux_s : std_logic_vector(img_width_g - 1 downto 0);

--signals for memory for sum2
signal sum2_data_s: std_logic_vector(result_width_g - 1 downto 0);

-- signal for memory eneable
signal en_mem_s: std_logic;

signal en_delay1_reg, en_delay2_reg, en_delay3_reg, en_delay1_next, en_delay2_next, en_delay3_next : std_logic;

--signal bram_ctrl,  
signal bram_ctrl_sel_inv_s, bram_ctrl_sel_reg, bram_ctrl_sel_next, we_tmpl_mux_s, we_img_mux_s :std_logic_vector(3 downto 0); -- 0 IP uses port , 1 CPU uses port.

--register for ready
signal ready_reg, ready_next, start_reg: std_logic;
signal start_next: std_logic;

--signal for reset system
signal reset_system : std_logic;

signal zero_4 : std_logic_vector(3 downto 0);
signal zero_width_g : std_logic_vector(pixel_width_g-1 downto 0);

--signals from bram ctrl to bram
signal img_addr_i : std_logic_vector(img_width_g + tmpl_width_g -1 downto 0);
signal tmpl_addr_i : std_logic_vector(tmpl_width_g + tmpl_width_g  -1 downto 0);
signal sum1_addr_i : std_logic_vector(img_width_g -1 downto 0);
signal resp_addr_i : std_logic_vector(img_width_g -1 downto 0);
           
signal img_data_s, tmpl_data_s :std_logic_vector(pixel_width_g - 1 downto 0);

begin

img_addr_i <= img_addr_bram_ctrl_i(img_width_g + tmpl_width_g +2 -1 downto 2);
tmpl_addr_i <= tmpl_addr_bram_ctrl_i(tmpl_width_g + tmpl_width_g +2  -1 downto 2);

resp_addr_i <= resp_addr_bram_ctrl_i(img_width_g + 2 -1 downto 2);
sum1_addr_i <= sum1_addr_bram_ctrl_i(img_width_g + 2 -1 downto 2);

--zero signals
zero_4 <= std_logic_vector(to_unsigned(0,4));
zero_width_g <= std_logic_vector(to_unsigned(0,pixel_width_g));

--reset system (CPU reset or AXI LITE register reset)
reset_system <= reset or reset_stripe;

-- inverting bram_ctrl_sel_s
    bram_ctrl_sel_inv_s <= not (bram_ctrl_sel_reg);

    loop_c:loop_structure
    generic map(
                tmpl_width_g => tmpl_width_g,
                img_width_g => img_width_g
                )
    port map(
             clk => clk,
             reset => reset_system,
             en => en_reg,
             tmpl_cols_i => tmpl_cols_i,
             tmpl_rows_i => tmpl_rows_i,
             img_cols_i => img_cols_i,
             m_o => m_s,
             k_o => k_s,
             j_even_o => j_even_s,
             j_odd_o => j_odd_s,
             stripe_done_o => stripe_done_s,
             pixel_done_o => pixel_done_s
            );
           
    address_generation_c: address_generation
    generic map(
                tmpl_width_g => tmpl_width_g,
                img_width_g => img_width_g
                )
    port map(
             clk => clk,
             reset => reset_system,
             en => en_reg,
             tmpl_cols => tmpl_cols_i,
             img_cols => img_cols_i,
             m => m_s,
             k => k_s,
             j_even => j_even_s,
             j_odd => j_odd_s,
             tmpl_addr => tmpl_addr_s,
             img_even_addr => img_even_addr_s,
             img_odd_addr => img_odd_addr_s,
             sum1_even_addr => sum1_even_addr_s,
             sum1_odd_addr => sum1_odd_addr_s,
             resp_even_addr => resp_even_addr_s,
             resp_odd_addr => resp_odd_addr_s
             );
             
    mem_tmpl_c: memory_beh
    generic map(
                width_g => pixel_width_g,
                size_g => size_tmp_g
                )
    port map(
            clka => clk,
            clkb => clk,
            ena => en_mem_s,
            enb => en_mem_s,
            wea => zero_4, -- portA for IP read 
            web => we_tmpl_mux_s, -- portB for CPU write and IP read
            addra => tmpl_addr_s,
            addrb => tmpl_addr_i,
            dia => zero_width_g,
            dib => tmpl_data_s,
            doa => tmpl_pixel_data_s,
            dob => open
            );
            
tmpl_data_s <= tmpl_data_i(pixel_width_g - 1 downto 0);
 -- portA for IP, portB for CPU
 
    mem_img_c: memory_beh
    generic map(
                width_g => pixel_width_g,
                size_g => size_img_g
                )
    port map(
            clka => clk,
            clkb => clk,
            ena => en_mem_s,
            enb => en_mem_s,
            wea => we_img_mux_s,
            web => zero_4,
            addra => img_addr_mux_s,
            addrb => img_odd_addr_s,
            dia => img_data_s,
            dib => zero_width_g,
            doa => img_pixel_even_data_s,
            dob => img_pixel_odd_data_s
            );
            
img_data_s <= img_data_i(pixel_width_g - 1 downto 0);
-- portA using for IP/CPU, portB for IP  

    mem_resp_c:  memory_beh
    generic map(
                width_g => result_width_g,
                size_g => size_res_g
                )
    port map(
            clka => clk,
            clkb => clk,
            ena => en_mem_s,
            enb => en_mem_s,
            wea => bram_ctrl_sel_inv_s,
            web => bram_ctrl_sel_inv_s,
            addra => resp_addr_mux_s,
            addrb => resp_odd_addr_s,
            dia => resp_even_data_s,
            dib => resp_odd_data_s,
            doa => resp_data_o,
            dob => open
            ); 
-- portA using for IP/CPU, portB for IP
            
    mem_sum1_c:  memory_beh
    generic map(
                width_g => result_width_g,
                size_g => size_res_g
                )
    port map(
            clka => clk,
            clkb => clk,
            ena => en_mem_s,
            enb => en_mem_s,
            wea => bram_ctrl_sel_inv_s,
            web => bram_ctrl_sel_inv_s,
            addra => sum1_addr_mux_s,
            addrb => sum1_odd_addr_s,
            dia => sum1_even_data_s,
            dib => sum1_odd_data_s,
            doa => sum1_data_o,
            dob => open
            );
-- portA using for IP/CPU, portB for IP
                  
    calculation_c: calculation
    generic map(
                pixel_width_g => pixel_width_g,
                result_width_g => result_width_g
                )
    port map(
            clk => clk,
            reset => reset_system,
            en => en_delay3_reg,
            img_pixel_even_data => img_pixel_even_data_s,
            img_pixel_odd_data => img_pixel_odd_data_s,
            tmpl_pixel_data => tmpl_pixel_data_s,
            pixel_done => pixel_done_s,
            resp_even_data => resp_even_data_s,
            resp_odd_data => resp_odd_data_s,
            sum1_even_data => sum1_even_data_s,
            sum1_odd_data => sum1_odd_data_s,
            sum2_data => sum2_data_s
            ) ;  
            
ready_reg_p: process (clk)
            begin
                if rising_edge(clk) then
                     if (reset_system = '0') then
                        start_reg <= start_next;
                        ready_reg <= ready_next; 
                        bram_ctrl_sel_reg <= bram_ctrl_sel_next;
                        en_reg <= en_next;
                     else
                        start_reg <= '0';
                        ready_reg <= '1';
                        bram_ctrl_sel_reg <= "1111";
                        en_reg <= '0';
                     end if;
                end if;
            end process;
            
start_reg_p: process (start)
             begin           
                start_next <= start;
             end process;
             
-- process for control/status signal
ready_logic_p: process (start_reg, stripe_done_s, bram_ctrl_sel_reg, ready_reg, en_reg)
               begin
                    if (start_reg = '1') then
                        ready_next <= '0';
                        bram_ctrl_sel_next <= "0000";
                        en_next <= '1';
--                    elsif (start /= '0') then
--                        ready_next <= '1';
--                        en_next <= '0';
--                        bram_ctrl_sel_next <= '1';
                    elsif (stripe_done_s = '1') then
                        ready_next <= '1';
                        bram_ctrl_sel_next <= "1111";
                        en_next <= '0';
                    else
                        ready_next <= ready_reg;
                        bram_ctrl_sel_next <= bram_ctrl_sel_reg;
                        en_next <= en_reg;
                    end if;
               
               end process;

en_mem_s <= '1';

--process for multiplexing communication BRAM/IP
process_mux_p: process(bram_ctrl_sel_reg,img_addr_i,resp_addr_i,sum1_addr_i,img_even_addr_s,resp_even_addr_s,sum1_even_addr_s)
               begin
                    if(bram_ctrl_sel_reg = "1111")then --CPU controls
                        img_addr_mux_s <= img_addr_i;
                        resp_addr_mux_s <= resp_addr_i;
                        sum1_addr_mux_s <= sum1_addr_i;
                        we_img_mux_s <= we_img_i; 
                        we_tmpl_mux_s <= we_tmpl_i; 
                    else -- IP contorls
                        img_addr_mux_s <= img_even_addr_s;
                        resp_addr_mux_s <= resp_even_addr_s;
                        sum1_addr_mux_s <= sum1_even_addr_s;
                        we_img_mux_s <= "0000";
                        we_tmpl_mux_s <= "0000";
                    end if;
               end process;

process (clk)
        begin
            if rising_edge(clk) then
                if (reset_system = '0') then
                   if (en_reg = '1') then
                        en_delay1_reg <= en_delay1_next; 
                        en_delay2_reg <= en_delay2_next; 
                        en_delay3_reg <= en_delay3_next; 
                   else
                        en_delay1_reg <= '0'; 
                        en_delay2_reg <= '0';     
                        en_delay3_reg <= '0';     
                   end if;   
                else
                      en_delay1_reg <= '0'; 
                      en_delay2_reg <= '0';  
                      en_delay3_reg <= '0';  
                end if;
            end if;
    end process; 
    
en_delay1_next <= en_reg;
en_delay2_next <= en_delay1_reg;
en_delay3_next <= en_delay2_reg;

-- register for ouput data sum2 
process (clk)
            begin
                if rising_edge(clk) then
                     if (reset_system = '0') then
                        if(en_reg = '1')then
                            sum2_data_reg <= sum2_data_next;
                        end if;
                     else
                            sum2_data_reg <= (others => '0');
                     end if;
                end if;
            end process; 

sum2_data_next <= sum2_data_s;
            
--Ouput signals
ready_o <= ready_reg;
sum2_data_o <= sum2_data_reg;
end structure;


