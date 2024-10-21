# BSS Protocol in C++

## Overview

This project implements the **BSS Protocol** in C++, where multiple processes communicate in a distributed system using a vector clock to maintain the order of send and receive operations. The protocol simulates how each process broadcasts messages and receives them through a BSS process, ensuring that the messages are delivered to the application in the correct order.

## Features

- **Broadcast (send) operations**: Processes broadcast messages to all other processes.
- **Receive operations**: Messages are received first by the BSS process (`recv_B`), which ensures their proper ordering before delivering them to the application (`recv_A`).
- **Vector Clocks**: Each process maintains a vector clock to ensure that the causal relationships between operations are preserved.

## Input Format

The program takes as input a description of process schedules that include:
- **send**: Broadcast a message to all processes.
- **recv_B**: Receive a message from the BSS process.
- **recv_A**: Deliver a message from the BSS process to the application.

For each process, the program outputs the sequence of operations along with the updated vector clock at each step.

## Example

### Input:
begin process p1  
send m1  
send m2  
send m3  
end process p1  
  
begin process p2  
recv_B p1 m1  
send m4  
end process p2  

begin process p3  
recv_B p1 m2  
recv_B p2 m4  
end process p3  

begin process p4  
recv_B p1 m3  
end process p4  

### Output:

begin process p1:  
send m1 [1 0 0 0]  
send m2 [2 0 0 0]  
send m3 [3 0 0 0]  
end process p1  
  
begin process p2:  
recv_B p1 m1 [0 0 0 0]  
recv_A p1 m1 [1 0 0 0]  
send m4 [1 1 0 0]  
end process p2  

begin process p3:  
recv_B p1 m2 [0 0 0 0]  
recv_B p2 m4 [0 0 0 0]  
end process p3  

begin process p4:  
recv_B p1 m3 [0 0 0 0]  
end process p4  



## How to Run
1. **Modify the main function**:  
   Edit the number of processes and the input file name.

2. **Compile the code**:

   Run the following command to compile the C++ program:
   ```bash
   g++ -o program myBSSProg_21114103.cpp
   ./program
