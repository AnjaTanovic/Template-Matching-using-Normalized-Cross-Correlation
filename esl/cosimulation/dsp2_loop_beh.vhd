----------------------------------------------------------------------------------
-- File : dsp2_loop_beh 
-- Project: Template matching
-- Create Date: 05/31/2022 01:43:19 PM
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

entity dsp2_loop_beh is
    generic(
            width_g:POSITIVE:=8
            );
    Port( 
        clk : in std_logic;
        en : in std_logic;
        reset : in std_logic;
        
        -- input of dsp, we have 3.
        dsp1_i : in std_logic_vector(width_g-1 downto 0);
        dsp2_i : in std_logic_vector(width_g-1 downto 0);
        dsp3_i : in std_logic_vector(width_g-1 downto 0);
        
        -- output of dsp, 
        dsp1_o : out  std_logic_vector(width_g-1 downto 0);
        dsp3_o : out  std_logic_vector(width_g-1 downto 0);
        dsp2_o : out std_logic
        );
end dsp2_loop_beh;

architecture beh of dsp2_loop_beh is

-- instance of dsp 
attribute use_dsp : string;
attribute use_dsp of beh : architecture is "yes";

-- registers for dsp, one for increment, another for value
signal inc_s, inc_next_s, value_s, value_next_s : std_logic_vector(width_g -1 downto 0);

begin

-- process for registers
process_reg_p:  process (clk, reset, en)
                begin
                if rising_edge(clk) then
                    if (reset = '0') then
                        if (en = '1') then
                            --inc_s <= inc_next_s;
                            value_s <= value_next_s;     
                        end if;   
                    else
                        --inc_s <= (others => '0');
                        value_s <= (others => '0');        
                    end if;
                end if;
                end process;
               

-- process for add
process_add_p: process(dsp2_i,value_s)
               begin
                    inc_next_s <= std_logic_vector(unsigned(dsp2_i) + unsigned(value_s));
               end process;               

-- process for comp
process_comp_p: process(inc_next_s,dsp3_i) 
                begin
                    if(inc_next_s >= dsp3_i)then
                        dsp2_o <= '1';
                    else 
                        dsp2_o <= '0';    
                    end if;
                end process;                   
                
-- combinational path
value_next_s <= dsp1_i;
dsp1_o <= value_s;                 
dsp3_o <= inc_next_s;                
end beh;


