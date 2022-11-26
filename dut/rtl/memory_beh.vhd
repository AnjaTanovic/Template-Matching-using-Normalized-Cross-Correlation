----------------------------------------------------------------------------------
-- File : memory_beh 
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
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.utils_pkg.all;

entity memory_beh is
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
end memory_beh;

architecture beh of memory_beh is

type ram_type is array (size_g-1 downto 0) of std_logic_vector(width_g-1 downto 0);
signal RAM : ram_type;

attribute ram_style:string;
attribute ram_style of RAM: signal is "block";

begin

process(clka, clkb)
begin
    if (rising_edge(clka)) then

        if (ena = '1') then
            doa <= RAM(to_integer(unsigned(addra)));

            if (wea /= "0000") then
                RAM(to_integer(unsigned(addra))) <= dia;
            end if;
        end if;
    end if;
    
    if (rising_edge(clkb)) then
    
        if (enb = '1') then
            dob <= RAM(to_integer(unsigned(addrb)));
            
            if (web /= "0000") then
                RAM(to_integer(unsigned(addrb))) <= dib;
                
            end if;
        end if;
    end if;
end process;



end beh;
