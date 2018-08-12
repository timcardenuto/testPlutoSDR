----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08/11/2018 11:31:36 PM
-- Design Name: 
-- Module Name: multiply_tb - Behavioral
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

entity multiply_tb is
--  Port ( );
end multiply_tb;

architecture Behavioral of multiply_tb is

    Component multiply
    Port( data_in : in STD_LOGIC_VECTOR (31 downto 0);
        data_out : out STD_LOGIC_VECTOR (31 downto 0);
        valid_in : in STD_LOGIC;
        valid_out : out STD_LOGIC;
        clk : in STD_LOGIC);
    End Component;
    
    -- I = 5 and Q = 13
    signal data_in : std_logic_vector (31 downto 0) := "0000000000000101" & "0000000000001101";
    signal valid_in : std_logic := '0';
    signal clk : std_logic := '0';
    signal valid_out : std_logic;
    signal data_out : std_logic_vector (31 downto 0);

begin

    uut : multiply Port Map (
        data_in => data_in,
        data_out => data_out,
        valid_in => valid_in,
        valid_out => valid_out,
        clk => clk);

    stim : process
    begin
        wait for 100 ns;
        clk <= '1';
        -- input 0005000D; output should be 000A001A
        data_in <= "0000000000000101" & "0000000000001101";
        valid_in <= '1';
        wait for 100 ns;
        clk <= '0';
        valid_in <= '0';
        wait for 100 ns;
        clk <= '1';
        -- input 0045084D; output should be 008A109A
        data_in <= "0000000001000101" & "0000100001001101";
        valid_in <= '1';
        wait for 100 ns;
        clk <= '0';
        valid_in <= '0';
        wait for 100 ns;
        clk <= '1';
        -- input 1405040D; output should be 280A081A
        data_in <= "0001010000000101" & "0000010000001101";
        valid_in <= '1';
        wait for 100 ns;
        clk <= '0';
        valid_in <= '0';
        wait;
    end process;

end Behavioral;
