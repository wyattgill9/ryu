#include "keymaps.hpp"

#include <iostream>
#include <termios.h>
#include <unistd.h>

void enableRawMode() {
    termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

const char* key_to_string(NormalKeys key) {
    switch (key) {
        case INSERT: return "Insert";
        case MOVE_UP: return "Move up";
        case MOVE_DOWN: return "Move down";
        case MOVE_LEFT: return "Move left";
        case MOVE_RIGHT: return "Move right";
        case MOVE_NEXT_WORD: return "Move next word";
        case MOVE_PREVIOUS_WORD: return "Move previous word";
        case MOVE_END_OF_WORD: return "Move end of word";
        case MOVE_BEGINNING_OF_LINE: return "Move beginning of line";
        case MOVE_END_OF_LINE: return "Move end of line";
        case DELETE: return "Delete";
        case UNDO: return "Undo";
        case REDO: return "Redo";
        case SAVE: return "Save";
        case QUIT: return "Quit";
        case UNKNOWN: return "Unknown";
    }
}

int main() {
    enableRawMode();
    bool running = true;
    Mode current_mode = NORMAL_MODE;

    while (running) {
        if (current_mode == NORMAL_MODE) {
            NormalKeys k = parse_normal_key();
            if (k == INSERT) {
                current_mode = INSERT_MODE;
            } else {
                std::cout << key_to_string(k) << "\n";
            }
        }
        else { 
            char c;
            if (read(STDIN_FILENO, &c, 1) != 1) continue;

            if (c == 27) {
                current_mode = NORMAL_MODE;  
            } 
            else {
                std::cout << c;           
                std::cout.flush();
            }
        }
    }
}

