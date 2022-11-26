----------------------------------------------------------------------------------
-- File : loop_structure.vhd 
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

entity loop_structure is
    
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
end loop_structure;

architecture structure of loop_structure is

-- to map on dsp
attribute use_dsp : string;
attribute use_dsp of structure : architecture is "yes";

component dsp_loop_beh
    generic(
            width_g: POSITIVE := 8
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
end component;

component dsp2_loop_beh
    generic(
            width_g: POSITIVE := 8
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
end component;

-- signals for m loop
signal m_add_s, m_mux_s, m_inc_s : std_logic_vector(tmpl_width_g-1 downto 0);
signal m_sel_s : std_logic;
-- signals for k loop
signal k_add_s, k_mux_s, k_inc_s : std_logic_vector(tmpl_width_g-1 downto 0);
signal k_sel_s : std_logic;
-- signals for j even loop
signal j_even_add_s, j_even_mux_s,j_even_s, j_even_inc_s, sub_cols_s : std_logic_vector(img_width_g-1 downto 0);
signal j_even_sel_s : std_logic;

signal tmpl_cols_s, tmpl_cols_next, tmpl_rows_s, tmpl_rows_next : std_logic_vector(tmpl_width_g-1 downto 0);
signal img_cols_s, img_cols_next : std_logic_vector(img_width_g-1 downto 0);

signal stripe_done_s, stripe_done_delay1_reg, stripe_done_delay1_next,stripe_done_delay2_reg, stripe_done_delay2_next,stripe_done_delay3_reg, stripe_done_delay3_next,stripe_done_delay4_reg, stripe_done_delay4_next, stripe_done_delay5_reg, stripe_done_delay5_next : std_logic;
signal pixel_done_s, pixel_done_delay1_reg, pixel_done_delay1_next,pixel_done_delay2_reg, pixel_done_delay2_next,pixel_done_delay3_reg, pixel_done_delay3_next,pixel_done_delay4_reg, pixel_done_delay4_next : std_logic;

begin

-- instance of component

dsp_m: dsp_loop_beh
        generic map(
                    width_g => tmpl_width_g
                    )
        port map(
                 clk => clk,
                 reset => reset,
                 en => en,
                 dsp3_i => tmpl_cols_s,
                 dsp2_i => m_add_s,
                 dsp1_i => m_mux_s,
                 dsp1_o => m_o,
                 dsp2_o => m_sel_s,
                 dsp3_o => m_inc_s   
                 ); 
                      
dsp_k: dsp_loop_beh
        generic map(
                    width_g => tmpl_width_g
                    )
        port map(
                 clk => clk,
                 reset => reset,
                 en => en,
                 dsp3_i => tmpl_rows_s,
                 dsp2_i => k_add_s,
                 dsp1_i => k_mux_s,
                 dsp1_o => k_o,
                 dsp2_o => k_sel_s,
                 dsp3_o => k_inc_s   
                 ); 
                  
dsp_j_even: dsp2_loop_beh
        generic map(
                    width_g => img_width_g
                    )
        port map(
                 clk => clk,
                 reset => reset,
                 en => en,
                 dsp3_i => sub_cols_s,
                 dsp2_i => j_even_add_s,
                 dsp1_i => j_even_mux_s,
                 dsp1_o => j_even_s,
                 dsp2_o => j_even_sel_s,
                 dsp3_o => j_even_inc_s   
                 );
process(tmpl_cols_i, img_cols_i, tmpl_rows_i) 
begin
    tmpl_cols_next <= std_logic_vector(unsigned(tmpl_cols_i) - TO_UNSIGNED(1,tmpl_width_g));
    tmpl_rows_next <= std_logic_vector(unsigned(tmpl_rows_i) - TO_UNSIGNED(1,tmpl_width_g));
    img_cols_next <= std_logic_vector(unsigned(img_cols_i));

end process; 
-- process for sub
sub_process_p: process(tmpl_cols_s, img_cols_s)
               begin
                    sub_cols_s <= std_logic_vector(unsigned(img_cols_s) - unsigned(tmpl_cols_s));
               end process;
                   
-- combinational logic --                   
stripe_done_s <= '1' when m_sel_s ='1' and k_sel_s ='1' and j_even_sel_s ='1' else
           '0';
pixel_done_s <= '1' when m_sel_s ='1' and k_sel_s ='1' else
                '0';  
                
m_mux_s <= m_inc_s when m_sel_s = '0' else 
           (others => '0');
k_mux_s <= (others => '0') when m_sel_s = '1' and k_sel_s = '1' else 
           k_inc_s;
j_even_mux_s <= (others => '0') when m_sel_s = '1' and k_sel_s = '1'  and j_even_sel_s = '1' else 
           j_even_inc_s;

m_add_s <= std_logic_vector(to_unsigned(1, tmpl_width_g));
           
k_add_s <= std_logic_vector(to_unsigned(1, tmpl_width_g)) when m_sel_s = '1' else
           (others => '0');
j_even_add_s <= std_logic_vector(to_unsigned(2, img_width_g)) when k_sel_s = '1' and m_sel_s = '1' else
           (others => '0');
           
j_odd_o <= std_logic_vector(unsigned(j_even_s) + to_unsigned(1, img_width_g));
j_even_o <= j_even_s; 

    process (clk)
        begin
            if rising_edge(clk) then
                if (reset = '0') then
                   if (en = '1') then
                        stripe_done_delay1_reg <= stripe_done_delay1_next;
                        stripe_done_delay2_reg <= stripe_done_delay2_next;
                        stripe_done_delay3_reg <= stripe_done_delay3_next;
                        stripe_done_delay4_reg <= stripe_done_delay4_next;
                        stripe_done_delay5_reg <= stripe_done_delay5_next;
                        pixel_done_delay1_reg <=  pixel_done_delay1_next;
                        pixel_done_delay2_reg <=  pixel_done_delay2_next;
                        pixel_done_delay3_reg <=  pixel_done_delay3_next;
                        pixel_done_delay4_reg <=  pixel_done_delay4_next;
                        tmpl_cols_s <= tmpl_cols_next;
                        tmpl_rows_s <= tmpl_rows_next;
                        img_cols_s <= img_cols_next;
                   end if;   
                else
                        stripe_done_delay1_reg <= '0';
                        stripe_done_delay3_reg <= '0';
                        stripe_done_delay2_reg <= '0';
                        stripe_done_delay4_reg <= '0';
                        stripe_done_delay5_reg <= '0';
                        pixel_done_delay1_reg <= '0';
                        pixel_done_delay2_reg <= '0';
                        pixel_done_delay3_reg <= '0';
                        pixel_done_delay4_reg <= '0';
                        tmpl_cols_s <= (others => '1');
                        tmpl_rows_s <= (others => '1');
                        img_cols_s <= (others => '1');
                end if;
            end if;
    end process; 
    
    process (stripe_done_s,stripe_done_delay1_reg,stripe_done_delay2_reg,stripe_done_delay3_reg,pixel_done_s,pixel_done_delay1_reg,pixel_done_delay2_reg,pixel_done_delay3_reg)
    begin
            stripe_done_delay1_next <= stripe_done_s;
            stripe_done_delay2_next <= stripe_done_delay1_reg;
            stripe_done_delay3_next <= stripe_done_delay2_reg;
            stripe_done_delay4_next <= stripe_done_delay3_reg;
            stripe_done_delay5_next <= stripe_done_delay4_reg;
            
            pixel_done_delay1_next <= pixel_done_s;
            pixel_done_delay2_next <= pixel_done_delay1_reg;
            pixel_done_delay3_next <= pixel_done_delay2_reg;
            pixel_done_delay4_next <= pixel_done_delay3_reg;
    end process;
    
stripe_done_o <= stripe_done_delay5_reg;                                                                           
pixel_done_o <= pixel_done_delay4_reg;  
                                                                         
end structure;


