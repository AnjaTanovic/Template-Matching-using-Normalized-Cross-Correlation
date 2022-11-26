----------------------------------------------------------------------------------
-- File : calculation_rtl.vhd 
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
use IEEE.NUMERIC_STD.ALL;

entity calculation is
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
end calculation;

architecture rtl of calculation is

--------------------------------------------------------------------------------------------------------
    --registers for output data
    signal resp_even_mult_reg, resp_even_mult_next : std_logic_vector(2*pixel_width_g - 1 downto 0);
    signal resp_even_sum_reg, resp_even_sum_next : std_logic_vector(result_width_g - 1 downto 0);
    
    signal resp_odd_mult_reg, resp_odd_mult_next : std_logic_vector(2*pixel_width_g - 1 downto 0);
    signal resp_odd_sum_reg, resp_odd_sum_next : std_logic_vector(result_width_g - 1 downto 0);
    
    signal sum1_even_mult_reg, sum1_even_mult_next : std_logic_vector(2*pixel_width_g - 1 downto 0);
    signal sum1_even_sum_reg, sum1_even_sum_next : std_logic_vector(result_width_g - 1 downto 0);
    
    signal sum1_odd_mult_reg, sum1_odd_mult_next : std_logic_vector(2*pixel_width_g - 1 downto 0);
    signal sum1_odd_sum_reg, sum1_odd_sum_next : std_logic_vector(result_width_g - 1 downto 0);
    
    signal sum2_mult_reg, sum2_mult_next : std_logic_vector(2*pixel_width_g - 1 downto 0);
    signal sum2_sum_reg, sum2_sum_next : std_logic_vector(result_width_g - 1 downto 0);
    
    signal pixel_delay_reg, pixel_delay_next : std_logic;
    
    --multiplexers for operands in multiplication
    signal mux_sum_resp_even : std_logic_vector(result_width_g - 1 downto 0);
    signal mux_sum_resp_odd : std_logic_vector(result_width_g - 1 downto 0);
    signal mux_sum_sum1_even : std_logic_vector(result_width_g - 1 downto 0);
    signal mux_sum_sum1_odd : std_logic_vector(result_width_g - 1 downto 0);
    signal mux_sum_sum2 : std_logic_vector(result_width_g - 1 downto 0);
--------------------------------------------------------------------------------------------------------
    -- Atributes that need to be defined so Vivado synthesizer maps appropriate
    -- code to DSP cells
    attribute use_dsp : string;
    attribute use_dsp of rtl : architecture is "yes";
--------------------------------------------------------------------------------------------------------

begin

    register_synchronisation_p:
    process (clk, reset, en)
        begin
            if rising_edge(clk) then
                if (reset = '0') then
                   if (en = '1') then
                        resp_even_mult_reg <= resp_even_mult_next;
                        resp_even_sum_reg <= resp_even_sum_next;
                        resp_odd_mult_reg <= resp_odd_mult_next;
                        resp_odd_sum_reg <= resp_odd_sum_next;
                        sum1_even_mult_reg <= sum1_even_mult_next; 
                        sum1_even_sum_reg <= sum1_even_sum_next;
                        sum1_odd_mult_reg <= sum1_odd_mult_next; 
                        sum1_odd_sum_reg <= sum1_odd_sum_next;
                        sum2_mult_reg <= sum2_mult_next;
                        sum2_sum_reg <= sum2_sum_next;
                        pixel_delay_reg <= pixel_delay_next;
                   end if;   
                else
                    resp_even_mult_reg <= (others => '0');
                    resp_even_sum_reg <= (others => '0');
                    resp_odd_mult_reg <= (others => '0');
                    resp_odd_sum_reg <= (others => '0');
                    sum1_even_mult_reg <= (others => '0');
                    sum1_even_sum_reg <= (others => '0');
                    sum1_odd_mult_reg <= (others => '0');
                    sum1_odd_sum_reg <= (others => '0');
                    sum2_mult_reg <= (others => '0');
                    sum2_sum_reg <= (others => '0');
                    pixel_delay_reg <= '0';
                end if;
            end if;
    end process;
    
    --process for calculating results
    process (pixel_done, tmpl_pixel_data, img_pixel_even_data, img_pixel_odd_data, pixel_delay_reg, resp_even_mult_reg,
    resp_even_sum_reg, resp_odd_mult_reg, resp_odd_sum_reg, sum1_even_mult_reg, sum1_even_sum_reg, sum1_odd_mult_reg,
    sum1_odd_sum_reg, sum2_mult_reg, sum2_sum_reg, mux_sum_resp_even, mux_sum_resp_odd, mux_sum_sum1_even, mux_sum_sum1_odd, mux_sum_sum2) 
    begin
        
        pixel_delay_next <= pixel_done;
        
        --Multiplication
        resp_even_mult_next <= std_logic_vector(signed(tmpl_pixel_data) * signed(img_pixel_even_data));
        resp_odd_mult_next <= std_logic_vector(signed(tmpl_pixel_data) * signed(img_pixel_odd_data));
        
        sum1_even_mult_next <= std_logic_vector(signed(img_pixel_even_data) * signed(img_pixel_even_data));
        sum1_odd_mult_next <= std_logic_vector(signed(img_pixel_odd_data) * signed(img_pixel_odd_data));
        
        sum2_mult_next <= std_logic_vector(signed(tmpl_pixel_data) * signed(tmpl_pixel_data));

        --Addition
        resp_even_sum_next <= std_logic_vector(signed(resp_even_mult_reg) + signed(mux_sum_resp_even));
        resp_odd_sum_next <= std_logic_vector(signed(resp_odd_mult_reg) + signed(mux_sum_resp_odd));
        
        sum1_even_sum_next <= std_logic_vector(signed(sum1_even_mult_reg) + signed(mux_sum_sum1_even));
        sum1_odd_sum_next <= std_logic_vector(signed(sum1_odd_mult_reg) + signed(mux_sum_sum1_odd));
        
        sum2_sum_next <= std_logic_vector(signed(sum2_mult_reg) + signed(mux_sum_sum2));
       
        
    end process;

    --Combinatoral logic (mux) for multiplication
    mux_sum_resp_even <= resp_even_sum_reg when pixel_delay_reg = '0' else
                        (others => '0');
    mux_sum_resp_odd <= resp_odd_sum_reg when pixel_delay_reg = '0' else
                        (others => '0');
    mux_sum_sum1_even <= sum1_even_sum_reg when pixel_delay_reg = '0' else
                        (others => '0');
    mux_sum_sum1_odd <= sum1_odd_sum_reg when pixel_delay_reg = '0' else
                        (others => '0');
    mux_sum_sum2 <= sum2_sum_reg when pixel_delay_reg = '0' else
                        (others => '0');   
                        
    --Output signals   
    resp_even_data <= resp_even_sum_reg;
    resp_odd_data <= resp_odd_sum_reg;
    sum1_even_data <= sum1_even_sum_reg;
    sum1_odd_data <= sum1_odd_sum_reg;
    sum2_data <= sum2_sum_reg;            
    
end rtl;



