/*
    Author: Nathan Laha
    Description: This is a simple driver for printing 
    characters and strings to the screen

    As with a lot of stuff in this OS kernel, a lot of credit goes to the following book:
    https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
*/

#include "screen.h"
#include "../kernel/low_level.h"
#include "../kernel/util.h"
#include "../lib/string.h"

int get_screen_offset(int col, int row);
int get_cursor();
void set_cursor(int offset);
int handle_scrolling(int cursor_offset);

// Description: Print a char on the screen at col, row or at cursor position
// Input: character, col, row, attribute
// Output: none
void print_char(char character, int col, int row, char attribute_byte)
{
    // Create a byte (char) pointer to the start of video memory
    unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;

    // If attribute byte is zero, assume the default style
    if (!attribute_byte)
    {
        attribute_byte = WHITE_ON_BLACK;
    }

    // Get the video memory offset for the screen location
    int offset;
    // If col and row are non-negative, use them for offset
    if (col >= 0 && row >= 0)
    {
        offset = get_screen_offset(col, row);

        // Otherwise use the current cursor position
    }
    else
    {
        offset = get_cursor();
    }

    // If we see a newline character, set offset to the end of
    // current row, so it will be advanced to the first col
    // of the next row.
    if (character == '\n')
    {
        int rows = offset / (2 * MAX_COLS);
        offset = get_screen_offset(79, rows);

        // Otherwise, write the character and its attribute byte to
        // video memory at our calculated offset.
    }
    else
    {
        vidmem[offset] = character;
        vidmem[offset + 1] = attribute_byte;
    }

    // Update the offset to the next character cell, which is
    // two bytes ahead of the current cell.
    offset += 2;
    // Make scrolling adjustment, for when we reach the bottom of the screen.
    offset = handle_scrolling(offset);

    // Update the cursor position on the screen device.
    set_cursor(offset);
}

// Description: get the offset of the screen location at col, row
// Input: col, row
// Output: offset
int get_screen_offset(int col, int row)
{
    return ((row * MAX_COLS) + col) * 2;
}

// Description: get the offset of the cursor
// Input: none
// Output: offset
int get_cursor()
{
    // The device uses its control register as an index
    // to select its internal registers, of which we are
    // interested in:
    //   reg 14: which is the high byte of the cursor's offset
    //   reg 15: which is the low byte of the cursor's offset
    // Once the internal register has been selected, we may read or
    // write a byte on the data register.
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    // Sincr the cursor offset reported by the VGA hardware is the
    // number of characters, we multiply by two to convert it to
    // a character cell offset.
    return offset * 2;
}

// Description: set the cursor offset
// Input: offset
// Output: none
void set_cursor(int offset)
{
    offset /= 2; // Covert from cell offset to char offset.

    // This is similar to get_cursor, only now we write
    // bytes to those internal device registers.
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, offset);
}

// Description: print a string on the screen at col, row or at cursor position
// Input: string, col, row, use_cursor, attribute
// Output: none
void print_at(char *message, int col, int row, int use_cursor, char attribute_byte)
{
    // Update the cursor if col and row are not negative.
    if (col >= 0 && row >= 0 && use_cursor == 1)
    {
        set_cursor(get_screen_offset(col, row));
    }

    // Loop through each char of the message and print it.
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i++], col + i, row, attribute_byte);
    }
}

// Description: print a string on the screen at cursor position
// Input: string, center, attribute
// Output: none
void print(char *message, unsigned int center, char attribute_byte)
{
    if (!attribute_byte)
    {
        attribute_byte = WHITE_ON_BLACK;
    }

    if (center == 0)
    {
        print_at(message, -1, -1, 1, attribute_byte);
    }
    else if (center == 1)
    {
        print_at(message, get_center_col(message), get_center_row(), 0, attribute_byte);
    }
}

// Description: print a string on the screen at cursor position with fancy logging style
// Input: string
// Output: none
void log(char *message)
{
    char *prefix = "[NOS Kernel]: ";
    print(prefix, 0, RED_ON_BLACK);
    char *postfix = "\n";
    strcat(message, postfix);
    print(message, 0, WHITE_ON_BLACK);
}

// Description: clear the screen
// Input: none
// Output: none
void clear_screen()
{
    int row = 0;
    int col = 0;

    // Loop through video memory and write blank characters
    for (row = 0; row < MAX_ROWS; row++)
    {
        for (col = 0; col < MAX_COLS; col++)
        {
            print_char(' ', col, row, WHITE_ON_BLACK);
        }
    }

    // Move the cursor back to the top left.
    set_cursor(get_screen_offset(0, 0));
}

// Description: handle scrolling
// Input: offset
// Output: offset
int handle_scrolling(int cursor_offset)
{
    // If the cursor is within the screen, return it unmodified.
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2)
    {
        return cursor_offset;
    }

    // Shuffle the rows back one.
    int i;
    for (i = 1; i < MAX_ROWS; i++)
    {
        memory_copy((char *)(uintptr_t)get_screen_offset(0, i) + VIDEO_ADDRESS,
                    (char *)(uintptr_t)get_screen_offset(0, i - 1) + VIDEO_ADDRESS,
                    MAX_COLS * 2);
    }

    // Blank the last line by setting all the bytes to 0
    char *last_line = (char *)(uintptr_t)get_screen_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS;
    for (i = 0; i < MAX_COLS * 2; i++)
    {
        last_line[i] = 0;
    }

    // Move the offset back one row, such that it is now on the last
    // row, rather than off the edge of the screen.
    cursor_offset -= 2 * MAX_COLS;

    // Return the updated cursor position.
    return cursor_offset;
}

// Description: get the center column of the screen
// Input: string
// Output: center column
int get_center_col(char *message)
{
    // get the length of the message and divide by two
    int length = sizeof(message) / 2;
    return (MAX_COLS / 2) - 1 - length;
}

// Descrtiption: get the center row of the screen
// Input: none
// Output: center row
int get_center_row(void)
{
    return (MAX_ROWS / 2) - 1;
}