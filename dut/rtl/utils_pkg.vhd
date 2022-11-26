----------------------------------------------------------------------------------
-- File : utilis_pkg.vhd
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
library ieee;
use ieee.std_logic_1164.all;

package utils_pkg is
    function log2c (n: integer) return integer;
end utils_pkg;

--package body
package body utils_pkg is

    function log2c (n: integer) return integer is
    variable m, p: integer;

begin
    
    m := 0;
    p := 1;
    
    while p < n loop
    
        m := m + 1;
        p := p * 2;
        
    end loop;
    
    return m;

end log2c;

end utils_pkg;