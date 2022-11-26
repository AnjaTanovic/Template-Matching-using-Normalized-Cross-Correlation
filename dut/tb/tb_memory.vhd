----------------------------------------------------------------------------------
-- File : tb_memory.vhd 
-- Project: Template matching
-- Create Date: 06/02/2022 01:05:19 PM
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
use work.utils_pkg.all;

entity tb_memory is
--  Port ( );
end tb_memory;

architecture Behavioral of tb_memory is
    
    component memory_beh is
    generic(
            width_g: integer := 32;
            size_g: integer := 16
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
    
    signal clk_s,en_s: std_logic;
    signal wea_s, web_s : std_logic_vector(3 downto 0);
    signal p1_addr_s,p2_addr_s: std_logic_vector(3 downto 0);
    signal p1_data_s, p2_data_s, p1_data_o_s, p2_data_o_s: std_logic_vector(31 downto 0);

begin
memory: memory_beh 
        port map
            (
             clka => clk_s,
             clkb => clk_s,
             ena => en_s,
             enb => en_s,
             wea => wea_s,
             web => web_s,
             addra =>  p1_addr_s,
             addrb => p2_addr_s,
             dia => p1_data_s,
             dib => p2_data_s,
             doa => p1_data_o_s,
             dob => p2_data_o_s     
            );
clk_process_p:process is
        begin
            clk_s <= '0', '1' after 50ns;
            wait for 100ns; 
        end process;
     
        
        
p1_addr_process_p:process is 
        begin
            for i in 0 to 7 loop
                  en_s <= '1';
                  wea_s <= "1111";
                   p1_addr_s <= std_logic_vector(TO_UNSIGNED(i,4));
                   p1_data_s <= std_logic_vector(TO_SIGNED(i + 8,32));
                 
                --p2_addr_s <= std_logic_vector(TO_UNSIGNED(i,10) + TO_UNSIGNED(1, 10));
                wait for 200ns;
            end loop;
        end process;
        
p2_addr_process_p:process is 
        begin
                for i in 0 to 7 loop
               --p1_addr_s <= std_logic_vector(TO_UNSIGNED(i,10));
                web_s <= "1111";
                en_s <= '1';
            
                p2_addr_s <= std_logic_vector(TO_UNSIGNED(i + 8,4) );
                p2_data_s <= std_logic_vector(TO_SIGNED(i,32));
                wait for 200ns;
            end loop;
        end process;       
        
--p1_data_process_p:process is 
--        begin
--            for i in 0 to 7 loop
--                p1_data_s <= std_logic_vector(TO_SIGNED(i,32));
--                --p2_data_s <= std_logic_vector(TO_UNSIGNED(i,32) + TO_UNSIGNED(1, 32));
--                wait for 200ns;
--            end loop;
--        end process;


--p2_data_process_p:process is 
--        begin
--            for i in 0 to 7 loop
--               --p1_data_s <= std_logic_vector(TO_UNSIGNED(i,32));
--                p2_data_s <= std_logic_vector(TO_SIGNED(i,32));
--                wait for 200ns;
--            end loop;
--       end process;
        
-- reset_process_p:process is 
--        begin
       
--        wea_s <= '1';
--        web_s <= '1';
--        wait;
--        end process;
end Behavioral;

