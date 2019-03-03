----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08/11/2018 09:41:31 PM
-- Design Name: 
-- Module Name: multiply - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity multiply is
    Port(clk       : in  std_logic;
         valid_in  : in  std_logic;
         data_in   : in  std_logic_vector(31 downto 0);
         valid_out : out std_logic;
         data_out  : out std_logic_vector(31 downto 0));
end multiply;

architecture Behavioral of multiply is
    signal data_out_sig   : std_logic_vector(31 downto 0) := (others=>'0');
    signal data_available : std_logic := '0';
begin

-- create process called 'mul' to multiply data_in
mul : process(clk)
begin
    if rising_edge(clk) then
        if (valid_in='1') then
            -- multiply I and Q values by 2, left shifting each by 1 bit
            data_out_sig <= data_in(30 downto 16) & '0' & data_in(14 downto 0) & '0';
            data_available <= '1';
        end if;
        
        if (data_available='1') then
            data_available <= '0';
        end if;
    end if;
end process;

data_out <= data_out_sig;
valid_out <= data_available;

end Behavioral;
