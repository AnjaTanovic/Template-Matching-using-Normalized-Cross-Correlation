----------------------------------------------------------------------------------
-- File : address_generation_rtl.vhd 
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

entity address_generation is
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
           resp_even_addr : out std_logic_vector(img_width_g - 1 downto 0);
           resp_odd_addr : out std_logic_vector(img_width_g - 1 downto 0));
end address_generation;

architecture rtl of address_generation is

--------------------------------------------------------------------------------------------------------
    --registers for reading addresses
    signal tmpl_addr_reg, tmpl_addr_next : std_logic_vector(tmpl_width_g + tmpl_width_g - 1 downto 0);
    signal img_even_addr_reg, img_even_addr_next : std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
    signal img_odd_addr_reg, img_odd_addr_next : std_logic_vector(img_width_g + tmpl_width_g - 1 downto 0);
    signal sum_even_reg, sum_even_next : std_logic_vector(img_width_g - 1 downto 0);
    signal sum_odd_reg, sum_odd_next : std_logic_vector(img_width_g - 1 downto 0);
    signal m_delay_reg, m_delay_next : std_logic_vector(tmpl_width_g - 1 downto 0);
    
    --delay registers for writing addresses (results)
    signal j_even_d1_reg, j_even_d1_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_even_d2_reg, j_even_d2_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_even_d3_reg, j_even_d3_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_even_d4_reg, j_even_d4_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_even_d5_reg, j_even_d5_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_d1_reg, j_odd_d1_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_d2_reg, j_odd_d2_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_d3_reg, j_odd_d3_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_d4_reg, j_odd_d4_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_d5_reg, j_odd_d5_next : std_logic_vector(img_width_g - 1 downto 0);
    
    --delay registers before multiplication and addition
    signal m_reg, m_next : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal k_reg, k_next : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal j_even_reg, j_even_next : std_logic_vector(img_width_g - 1 downto 0);
    signal j_odd_reg, j_odd_next : std_logic_vector(img_width_g - 1 downto 0);
    signal tmpl_cols_reg, tmpl_cols_next : std_logic_vector(tmpl_width_g - 1 downto 0);
    signal img_cols_reg, img_cols_next : std_logic_vector(img_width_g - 1 downto 0);
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
                        tmpl_addr_reg <= tmpl_addr_next;  
                        img_even_addr_reg <= img_even_addr_next;
                        img_odd_addr_reg <= img_odd_addr_next;
                        sum_even_reg <= sum_even_next;
                        sum_odd_reg <= sum_odd_next;
                        m_delay_reg <= m_delay_next;
                        j_even_d1_reg <= j_even_d1_next;
                        j_even_d2_reg <= j_even_d2_next;
                        j_even_d3_reg <= j_even_d3_next;
                        j_even_d4_reg <= j_even_d4_next;
                        j_even_d5_reg <= j_even_d5_next;
                        j_odd_d1_reg <= j_odd_d1_next;
                        j_odd_d2_reg <= j_odd_d2_next;
                        j_odd_d3_reg <= j_odd_d3_next;
                        j_odd_d4_reg <= j_odd_d4_next;
                        j_odd_d5_reg <= j_odd_d5_next;
                        m_reg <= m_next;
                        k_reg <= k_next;
                        j_even_reg <= j_even_next;
                        j_odd_reg <= j_odd_next;
                        tmpl_cols_reg <= tmpl_cols_next;
                        img_cols_reg <= img_cols_next;
                   end if;   
                else
                    tmpl_addr_reg <= (others => '0');
                    img_even_addr_reg <= (others => '0');
                    img_odd_addr_reg <= (others => '0');
                    sum_even_reg <= (others => '0');
                    sum_odd_reg <= (others => '0');
                    m_delay_reg <= (others => '0');
                    j_even_d1_reg <= (others => '0');
                    j_even_d2_reg <= (others => '0');
                    j_even_d3_reg <= (others => '0');
                    j_even_d4_reg <= (others => '0');
                    j_even_d5_reg <= (others => '0');
                    j_odd_d1_reg <= (others => '0');
                    j_odd_d2_reg <= (others => '0');
                    j_odd_d3_reg <= (others => '0');
                    j_odd_d4_reg <= (others => '0');
                    j_odd_d5_reg <= (others => '0');
                    m_reg <= (others => '0');
                    k_reg <= (others => '0');
                    j_even_reg <= (others => '0');
                    j_odd_reg <= (others => '0');
                    tmpl_cols_reg <= (others => '0');
                    img_cols_reg <= (others => '0');
                end if;
            end if;
    end process;
    
    address_generation_logic_p:
    process (tmpl_cols, tmpl_cols_reg, k, k_reg, img_cols, img_cols_reg, m, m_reg, j_even, j_even_reg, j_odd, j_odd_reg, j_odd_d1_reg, j_odd_d4_reg, j_odd_d2_reg, j_odd_d3_reg, j_even_d1_reg, j_even_d2_reg, j_even_d3_reg,j_even_d4_reg) 
        begin 
            tmpl_addr_next <= std_logic_vector(unsigned(tmpl_cols_reg) * unsigned(k_reg));
            
            img_even_addr_next <= std_logic_vector(unsigned(img_cols_reg) * unsigned(k_reg));
            sum_even_next <= std_logic_vector(unsigned(j_even_reg) + unsigned(m_reg));
            
            img_odd_addr_next <= std_logic_vector(unsigned(img_cols_reg) * unsigned(k_reg));
            sum_odd_next <= std_logic_vector(unsigned(j_odd_reg) + unsigned(m_reg));
    
            m_delay_next <= m_reg;
            
            j_even_d1_next <= j_even;
            j_even_d2_next <= j_even_d1_reg;
            j_even_d3_next <= j_even_d2_reg;
            j_even_d4_next <= j_even_d3_reg;
            j_even_d5_next <= j_even_d4_reg;
                       
            j_odd_d1_next <= j_odd;
            j_odd_d2_next <= j_odd_d1_reg;
            j_odd_d3_next <= j_odd_d2_reg;
            j_odd_d4_next <= j_odd_d3_reg;
            j_odd_d5_next <= j_odd_d4_reg;
            
            m_next <= m;
            k_next <= k;
            j_even_next <= j_even;
            j_odd_next <= j_odd;
            tmpl_cols_next <= tmpl_cols;
            img_cols_next <= img_cols;
    end process;
    
    --Output signals
    
    tmpl_addr <= std_logic_vector(unsigned(tmpl_addr_reg) + unsigned(m_delay_reg));
    img_even_addr <= std_logic_vector(unsigned(sum_even_reg) + unsigned(img_even_addr_reg));
    img_odd_addr <= std_logic_vector(unsigned(sum_odd_reg) + unsigned(img_odd_addr_reg));
    
    sum1_even_addr <= j_even_d5_reg;
    sum1_odd_addr <= j_odd_d5_reg;
    resp_even_addr <= j_even_d5_reg;
    resp_odd_addr <= j_odd_d5_reg;
    
end rtl;

