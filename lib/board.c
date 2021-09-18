/*
    Author: Nathan Laha
    Description: Provides functions for simulating 
    and displaying a Conway's Game of Life board.
*/

#include "board.h"
#include "../drivers/screen.h"

// define the game board
static int *board[BOARD_HEIGHT][BOARD_WIDTH];

// function clears and initializes the game board with some test data
// TODO: grab data from a text file or generate random data
// Input: none
// Output: none
void clear_board(void)
{
    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            board[i][j] = 0;

            if (i == 10)
            {
                if (j == 10 || j == 11 || j == 12)
                {
                    board[i][j] = 1;
                }
            }
        }
    }
}

// function prints the game board to the screen
// Input: none
// Output: none
void print_board(void)
{
    set_cursor(0);
    print_at("Conway's Game of Life\n", 0, 0, 0, RED_ON_BLACK);
    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            char cellchar = '.';

            if (board[i][j] == 1)
            {
                cellchar = '#';
            }

            print_char(cellchar, -1, -1, WHITE_ON_BLACK);
        }

        print("\n", 0, WHITE_ON_BLACK);
    }
}

// updates the game board to the next GOL generation
// Input: none
// Output: none
void update_board(void)
{
    int row, col;
    int *boardTemp[BOARD_HEIGHT][BOARD_WIDTH];
    for (row = 0; row < BOARD_HEIGHT; row++)
    {
        for (col = 0; col < BOARD_WIDTH; col++)
        {
            int i, j;
            int num_neighbors = 0;

            for (i = row - 1; i <= row + 1; i++)
            {
                for (j = col - 1; j <= col + 1; j++)
                {
                    if (i == row && j == col)
                    {
                        continue;
                    }

                    if (i < 0 || i >= BOARD_HEIGHT || j < 0 || j >= BOARD_WIDTH)
                    {
                        continue;
                    }

                    if (board[i][j] == 1)
                    {
                        num_neighbors++;
                    }
                }
            }
            /*  GOL RULES BELOW  */

            // Cell dies (1 or 0 neighbors)
            if (num_neighbors < 2)
            {
                boardTemp[row][col] = 0;
            }
            // Cell dies (> 3 neighbors)
            else if (num_neighbors > 3)
            {
                boardTemp[row][col] = 0;
            }
            // Call grows (erowactlcol 3 neighbors)
            else if (num_neighbors == 3 && board[row][col] == 0)
            {
                boardTemp[row][col] = 1;
            }
            // if it's not one of these things, it stays the same
            else
            {
                boardTemp[row][col] = board[row][col];
            }
        }
    }

    for (int i = 0; i < BOARD_HEIGHT; i++)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            board[i][j] = boardTemp[i][j];
        }
    }
}