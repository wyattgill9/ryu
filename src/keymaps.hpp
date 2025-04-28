#ifndef KEYMAPS_HPP
#define KEYMAPS_HPP

enum Mode {
    NORMAL_MODE,
    INSERT_MODE,
};

enum NormalKeys {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_NEXT_WORD,
    MOVE_PREVIOUS_WORD,
    MOVE_BEGINNING_OF_LINE,
    MOVE_END_OF_LINE,
    MOVE_END_OF_WORD,
    DELETE,
    UNDO,
    REDO,
    INSERT,
    SAVE,
    QUIT,
    UNKNOWN
};

NormalKeys parse_normal_key();

#endif 
