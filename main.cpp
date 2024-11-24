#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>
#include <unistd.h>

std::vector<std::vector<char>> checkLife(std::vector<std::vector<char>> mouseClicks);

// Function to compute the next state of the board
void computeNextState(const std::vector<std::vector<char>>& currentBoard,
                      std::vector<std::vector<char>>& nextBoard,
                      int startRow, int endRow, int startCol, int endCol);

// Function to count live neighbors
int countLiveNeighbors(const std::vector<std::vector<char>>& board, int row, int col);

std::mutex mtx;
std::condition_variable cv;
int threads_done = 0;

void workerThread(int id, const std::vector<std::vector<char>>& board,
                  std::vector<std::vector<char>>& nextBoard,
                  int startRow, int endRow, int startCol, int endCol) {
    computeNextState(board, nextBoard, startRow, endRow, startCol, endCol);

    // Signal thread completion
    std::unique_lock<std::mutex> lock(mtx);
    threads_done++;
    cv.notify_one();
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./GOL <board size> <num threads per dimension> <max generations>\n";
        return 1;
    }

    const int M = std::stoi(argv[1]);
    const int N = std::stoi(argv[2]);
    const int MAX = std::stoi(argv[3]);

    if (M % N != 0) {
        std::cerr << "Board size must be divisible by the number of threads per dimension.\n";
        return 1;
    }

    const int BLOCK_SIZE = M / N;
    std::vector<std::vector<char>> board(M, std::vector<char>(M, ' '));
    std::vector<std::vector<char>> nextBoard(M, std::vector<char>(M, ' '));

    // Initialize curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Input board configuration
    mousemask(ALL_MOUSE_EVENTS, NULL);
    MEVENT event;
    while (true) {
        int ch = getch();
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK && event.y < M && event.x < M) {
                board[event.y][event.x] = (board[event.y][event.x] == 'x') ? ' ' : 'x';
                mvwaddch(stdscr, event.y, event.x, board[event.y][event.x]);
                wrefresh(stdscr);
            }
        } else if (ch == 'q') {
            break;
        }
    }

    // Run for MAX generations
    for (int gen = 0; gen < MAX; ++gen) {
        threads_done = 0;
        std::vector<std::thread> threads;

        // Spawn worker threads
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int startRow = i * BLOCK_SIZE;
                int endRow = (i + 1) * BLOCK_SIZE;
                int startCol = j * BLOCK_SIZE;
                int endCol = (j + 1) * BLOCK_SIZE;

                threads.emplace_back(workerThread, i * N + j, std::cref(board),
                                     std::ref(nextBoard), startRow, endRow, startCol, endCol);
            }
        }

        // Wait for threads to finish
        // Wait for threads to finish
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&N] { return threads_done == N * N; });
}


        for (auto& t : threads) t.join();

        board.swap(nextBoard);

        // Display the board
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < M; ++j) {
                mvwaddch(stdscr, i, j, board[i][j]);
                
            }
        }
        wrefresh(stdscr);
        usleep(100000);
    }

    endwin();
    return 0;
}

std::vector<std::vector<char>> checkLife(std::vector<std::vector<char>> mouseClicks) {
    std::vector<std::vector<char>> newState = mouseClicks; // Create a copy to store the new state

    for (int i = 1; i < mouseClicks.size() - 1; i++) {           // Iterate over rows
        for (int j = 1; j < mouseClicks[i].size() - 1; j++) {   // Iterate over columns
            int neighbors = 0;

            //neighbors that are alive
            for (int ni = -1; ni <= 1; ni++) {
                for (int nj = -1; nj <= 1; nj++) {
                    if (ni == 0 && nj == 0) continue; // skip current cell
                    if (mouseClicks[i + ni][j + nj] == 'x') {
                        neighbors++;
                    }
                }
            }

            // Game of Life rules for a cell
            if (mouseClicks[i][j] == 'x') {
                if (neighbors < 2 || neighbors > 3) {
                    newState[i][j] = ' '; // Cell dies
                }
            } else {
                if (neighbors == 3) {
                    newState[i][j] = 'x'; // Cell becomes alive
                }
            }
        }
    }

    return newState; // Return the new state
}


int countLiveNeighbors(const std::vector<std::vector<char>>& board, int row, int col) {
    int liveCount = 0;
    int rows = board.size();
    int cols = board[0].size();

    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},          {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (const auto& dir : directions) {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
            if (board[newRow][newCol] == 'x') {
                liveCount++;
            }
        }
    }

    return liveCount;
}

void computeNextState(const std::vector<std::vector<char>>& currentBoard,
                      std::vector<std::vector<char>>& nextBoard,
                      int startRow, int endRow, int startCol, int endCol) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int liveNeighbors = countLiveNeighbors(currentBoard, i, j);
            if (currentBoard[i][j] == 'x') {
                nextBoard[i][j] = (liveNeighbors < 2 || liveNeighbors > 3) ? ' ' : 'x';
            } else {
                nextBoard[i][j] = (liveNeighbors == 3) ? 'x' : ' ';
            }
        }
    }
}
