#include "../drivers/screen.h"
#include "../lib/board.h"

void main()
{
    // Create a pointer to a char, and point it to the first text cell of
    // video memory (i.e. the top-left of the screen)
    //char* video_memory = (char*) 0xb8000;
    // At the address to by video_memory, store the character 'X'
    // (i.e. display 'X' in the top-left of the scren).
    //*video_memory = 'Y';

    log("Kernel bootloading complete!");
    log("Initializing kernel systems...");

    // TODO: Initialize the kernel's drivers and whatnot

    log("Starting Conway's Game of Life...");

    clear_board();
    clear_screen();
    while (1)
    {
        print_board();
        update_board();
    }
}
