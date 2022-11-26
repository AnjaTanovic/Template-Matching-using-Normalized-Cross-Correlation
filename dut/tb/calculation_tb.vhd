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

entity calculation_tb is
    Generic (result_width_g : positive := 32;
             pixel_width_g : positive := 8); 
end calculation_tb;

architecture tb of calculation_tb is

    signal clk : std_logic;
    signal reset :  std_logic;
    signal en :  std_logic;
    signal img_pixel_even_data : std_logic_vector(pixel_width_g - 1 downto 0);
    signal img_pixel_odd_data : std_logic_vector(pixel_width_g - 1 downto 0);
    signal tmpl_pixel_data : std_logic_vector(pixel_width_g - 1 downto 0);
    signal pixel_done : std_logic;
    signal resp_even_data : std_logic_vector(result_width_g - 1 downto 0);
    signal resp_odd_data : std_logic_vector(result_width_g - 1 downto 0);
    signal sum1_even_data : std_logic_vector(result_width_g - 1 downto 0);
    signal sum1_odd_data : std_logic_vector(result_width_g - 1 downto 0);
    signal sum2_data : std_logic_vector(result_width_g - 1 downto 0);
    
    component calculation is
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
begin

    --process for clk generation 
    clk_gen: process
    begin
        clk <= '0', '1' after 100 ns;
    wait for 200 ns;
    end process;

    --instantion of design
    duv: calculation
    generic map (result_width_g => result_width_g,
                pixel_width_g  => pixel_width_g)
    port map(clk => clk,
               reset => reset,
               en => en,
               img_pixel_even_data => img_pixel_even_data,
               img_pixel_odd_data => img_pixel_odd_data,
               tmpl_pixel_data => tmpl_pixel_data,
               pixel_done => pixel_done,
               resp_even_data => resp_even_data,
               resp_odd_data => resp_odd_data,
               sum1_even_data => sum1_even_data,
               sum1_odd_data => sum1_odd_data,
               sum2_data =>sum2_data);
               
    --Process for stimulus generation
    stim_gen: process
        --variables which implements loop
        variable a : integer := 0;
        variable b: integer := 0;
        variable c: integer := 0;
    begin
        -- system level reset
        reset <= '0', '1' after 250 ns, '0' after 400 ns;
        en <= '0', '1' after 350 ns;
        pixel_done <= '0', '1' after 3000 ns, '0' after 3300 ns;
        wait until falling_edge(clk);
        
        img_pixel_even_data <= std_logic_vector(to_signed(10,pixel_width_g));
        img_pixel_odd_data <= std_logic_vector(to_signed(-3,pixel_width_g));
        tmpl_pixel_data <= std_logic_vector(to_signed(6,pixel_width_g));
        wait until falling_edge(clk);
        wait until falling_edge(clk);
        
        img_pixel_even_data <= std_logic_vector(to_signed(2,pixel_width_g));
        img_pixel_odd_data <= std_logic_vector(to_signed(20,pixel_width_g));
        tmpl_pixel_data <= std_logic_vector(to_signed(7,pixel_width_g));
         
        img_pixel_even_data <= std_logic_vector(to_signed(-15,pixel_width_g)) after 4000 ns;
        img_pixel_odd_data <= std_logic_vector(to_signed(-20,pixel_width_g)) after 4000 ns;
        tmpl_pixel_data <= std_logic_vector(to_signed(7,pixel_width_g)) after 4000 ns;
        
        wait;
    end process;
      
end tb;
