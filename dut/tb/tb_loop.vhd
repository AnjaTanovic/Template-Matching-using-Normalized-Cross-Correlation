----------------------------------------------------------------------------------
-- File : tb.vhd 
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

entity tb_loop is
--  Port ( );
end tb_loop;

architecture Behavioral of tb_loop is
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

signal clk_s, reset_s, en_s, stripe_done_s, pixel_done_s : std_logic;
signal tmpl_cols_s, tmpl_rows_s, m_s,k_s : std_logic_vector(7 downto 0);
signal img_cols_s, j_even_s, j_odd_s : std_logic_vector(9 downto 0);
begin

loop_c: loop_structure
        port map(
                 clk => clk_s,
                 reset => reset_s,
                 en => en_s,
                 tmpl_rows_i => tmpl_rows_s,
                 tmpl_cols_i => tmpl_cols_s,
                 img_cols_i => img_cols_s,
                 m_o => m_s,
                 k_o => k_s,
                 j_even_o => j_even_s,
                 j_odd_o => j_odd_s,
                 stripe_done_o => stripe_done_s,
                 pixel_done_o => pixel_done_s
                 );
clk_p: process is
        begin
            clk_s <= '0', '1' after 50ns;
            wait for 100ns;
        end process;
        
sim_p: process is
       begin
            reset_s <= '1','0' after 200 ns;
            en_s <= '0', '1' after 1000 ns;
            tmpl_rows_s <= std_logic_vector(TO_UNSIGNED(10,8));
            tmpl_cols_s <= std_logic_vector(TO_UNSIGNED(12,8));
            img_cols_s <= std_logic_vector(TO_UNSIGNED(16,10));
            wait;
       end process;        
end Behavioral;
