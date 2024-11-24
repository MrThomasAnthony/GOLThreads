# GOLThreads
An extension of Conways Game of Life as demonstrated in Lab4 using a multi-threaded approach to compute the next state of the board. It divides the board into blocks and assigns each block to a separate thread for computation.

# GOL Threads - Ubuntu

To get started with the GOL Threads simulation, follow these steps:

5. **Install g++ and ncurses (if not already installed):**

   ```bash
   sudo apt update
   sudo apt install g++ libncurses5-dev libncursesw5-dev

6. **Compile the Game of Life program:**
    ```bash
    g++ main.cpp -o GOL -lncurses -pthread

7. **Run the GOL Threads program:**
    ```bash
    ./GOL 80 8 100
