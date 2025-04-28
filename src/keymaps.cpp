#include "keymaps.hpp"
#include <unistd.h>
#include <termios.h>

NormalKeys parse_normal_key() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return UNKNOWN;

    switch (c) {
        case 'i': return INSERT;
        case 'h': return MOVE_LEFT;
        case 'j': return MOVE_DOWN;
        case 'k': return MOVE_UP;
        case 'l': return MOVE_RIGHT;
        case 'w': return MOVE_NEXT_WORD;
        case 'b': return MOVE_PREVIOUS_WORD;
        case '0': return MOVE_BEGINNING_OF_LINE;
        case '$': return MOVE_END_OF_LINE;
        case 'e': return MOVE_END_OF_WORD;
        case 'u': return UNDO;
        case 18:  return REDO; // ASCII code for Ctrl+R
        case 'd': return DELETE;
        case ':': {
            if (read(STDIN_FILENO, &c, 1) != 1) return UNKNOWN;
            if (c == 'w') return SAVE;
            if (c == 'q') return QUIT;
            return UNKNOWN;
        }
        default: return UNKNOWN;
    }
}
