----------------------------------------------------------------------------------
-- File : address_generation_tb.vhd 
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


entity address_generation_tb is
    Generic (tmpl_width_g : positive := 8;
             img_width_g : positive := 10);
end address_generation_tb;

architecture tb of address_generation_tb is
    signal clk : std_logic;
    signal reset : std_logic;
    signal en : std_logic;
    signal tmpl_cols : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal img_cols : std_logic_vector(img_width_g - 1 downto 0);
    signal m : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal k : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal j_even : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd : std_logic_vector(img_width_g - 1 downto 0);
    signal tmpl_addr : std_logic_vector(tmpl_width_g + tmpl_width_g - 1 downto 0);
    signal img_even_addr : std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
    signal img_odd_addr : std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
    signal sum1_even_addr : std_logic_vector(img_width_g - 1 downto 0);
    signal sum1_odd_addr : std_logic_vector(img_width_g - 1 downto 0);
    signal resp_even_addr : std_logic_vector(img_width_g - 1 downto 0);
    signal resp_odd_addr : std_logic_vector(img_width_g - 1 downto 0);
    
    component address_generation is
        Port ( clk : in std_logic;
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
               resp_even_addr : out std_logic_vector(img_width_g - 1 downto 0);
               resp_odd_addr : out std_logic_vector(img_width_g - 1 downto 0));
    end component;
    
begin

    --process for clk generation 
    clk_gen: process
    begin
        clk <= '0', '1' after 100 ns;
    wait for 200 ns;
    end process;
    
    --instantion of design
    duv: address_generation 
    port map(  clk => clk,
               reset =>reset,
               en => en,
               tmpl_cols => tmpl_cols,
               img_cols  => img_cols,
               m => m,
               k => k,
               j_even => j_even,
               j_odd => j_odd,
               tmpl_addr =>  tmpl_addr,
               img_even_addr => img_even_addr,
               img_odd_addr => img_odd_addr,
               sum1_even_addr => sum1_even_addr,
               sum1_odd_addr => sum1_odd_addr,
               resp_even_addr => resp_even_addr,
               resp_odd_addr =>  resp_odd_addr);

    --Process for stimulus generation
    stim_gen: process
        --variables which implements loop
        variable a : integer := 0;
        variable b: integer := 0;
        variable c: integer := 0;
    begin
        -- system level reset
        reset <= '0', '1' after 250 ns, '0' after 400 ns;
        en <= '0', '1' after 270 ns;
        --tmpl_cols <= std_logic_vector(to_unsigned(140,tmpl_width_g));
        --img_cols <= std_logic_vector(to_unsigned(60,img_width_g));
        --m <= std_logic_vector(to_unsigned(45,tmpl_width_g));
        --k <= std_logic_vector(to_unsigned(5,tmpl_width_g));
        --j_even <= std_logic_vector(to_unsigned(10,img_width_g));
        --j_odd <= std_logic_vector(to_unsigned(11,img_width_g));
        
        for a in 0 to 699 loop
		  for b in 0 to 254 loop
		      for c in 0 to 254 loop
                  tmpl_cols <= std_logic_vector(to_unsigned(255,tmpl_width_g));
                  img_cols <= std_logic_vector(to_unsigned(955,img_width_g));
                  m <= std_logic_vector(to_unsigned(c,tmpl_width_g));
                  k <= std_logic_vector(to_unsigned(b,tmpl_width_g));
                  j_even <= std_logic_vector(to_unsigned(a,img_width_g));
                  j_odd <= std_logic_vector(to_unsigned(a+1,img_width_g));
                  wait until falling_edge(clk);
		      end loop;
		  end loop;
	   end loop;
        
        wait;
    end process;
    
end tb;
