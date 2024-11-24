# GOL Threads
An extension of Conways Game of Life as demonstrated in Lab4 using a multi-threaded approach to compute the next state of the board. It divides the board into blocks and assigns each block to a separate thread for computation.

# Ubuntu

To get started with the GOL Threads simulation, follow these steps:
1. **Clone the repository:**
   ```bash
   git clone https://github.com/MrThomasAnthony/GOLThreads.git
   cd GOLThreads

2. **Install g++ and ncurses (if not already installed):**

   ```bash
   sudo apt update
   sudo apt install g++ libncurses5-dev libncursesw5-dev

3. **Compile the Game of Life program:**
    ```bash
    g++ main.cpp -o GOL -lncurses -pthread

4. **Run the GOL Threads program:**
    ```bash
    ./GOL 80 8 100
