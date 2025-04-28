#include "buffer.hpp"
#include "keymaps.hpp"

#include <cstdlib>
#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

const char* COLOR_BG = "\x1b[48;2;36;40;59m";

void hide_cursor() {
    std::cout << "\x1b[?25l";
}

void show_cursor() {
    std::cout << "\x1b[?25h";
}

int get_terminal_rows() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

int get_terminal_cols() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void enableRawMode() {
    termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void clearScreen() {
    std::cout << "\x1b[2J";    
    std::cout << "\x1b[H";     
    std::cout << COLOR_BG;     
}

const char* mode_to_string(Mode mode) {
    switch (mode) {
        case NORMAL_MODE: return "NORMAL";
        case INSERT_MODE: return "INSERT";
        default: return "UNKNOWN";
    }
}

void drawStatusBar(const Buffer& buffer, Mode mode) {
    int term_cols = get_terminal_cols();
    int term_rows = get_terminal_rows();

    std::cout << "\x1b[" << term_rows << ";0H"; 
    std::cout << "\x1b[48;5;236m"; 

    std::string filename = buffer.get_filename().empty() ? "[No Name]" : buffer.get_filename();
    std::string modified = buffer.is_modified() ? " \x1b[31m[+]\x1b[0m" : "";

    std::string mode_str;
    switch (mode) {
        case NORMAL_MODE: mode_str = "\x1b[1;32mNORMAL\x1b[0m"; break;
        case INSERT_MODE: mode_str = "\x1b[1;34mINSERT\x1b[0m"; break;
        default: mode_str = "\x1b[1;31mUNKNOWN\x1b[0m"; break;
    }

    std::cout << " " << mode_str
              << " \x1b[38;5;244m|\x1b[0m " << filename << modified
              << " \x1b[38;5;244m|\x1b[0m Row: " << (buffer.get_cursor_row() + 1)
              << ", Col: " << (buffer.get_cursor_col() + 1);

    int len = 1 + mode_str.length() - 10 + 3 + filename.length() + modified.length() - 4 + 10;
    while (len++ < term_cols) {
        std::cout << " ";
    }

    std::cout << "\x1b[0m"; 
}

void printBuffer(const Buffer& buffer) {
    clearScreen();
    
    const std::string filename = buffer.get_filename().empty() ? "[No Name]" : buffer.get_filename();
    const bool is_modified = buffer.is_modified();
    const auto& content = buffer.get_content();
    const int cursor_row = buffer.get_cursor_row();
    const int cursor_col = buffer.get_cursor_col();

    std::cout << "\x1b[1;36mRyu Editor\x1b[0m - \x1b[1m" << filename 
              << (is_modified ? " \x1b[31m[+]\x1b[0m" : "\x1b[0m") << "\n\n";

    for (size_t i = 0; i < content.size(); ++i) {
        const std::string& line = content[i];
        
        // Line numbers
        std::cout << "\x1b[38;5;240m" << std::setw(3) << (i+1) << " \x1b[0m";

        for (size_t j = 0; j < line.length(); ++j) {
            if (i == cursor_row && j == cursor_col) {
                std::cout << "\x1b[30;47m" << line[j] << "\x1b[0m";
            } else {
                std::cout << line[j];
            }
        }

        if (i == cursor_row && cursor_col >= line.length()) {
            std::cout << "\x1b[30;47m \x1b[0m";
        }

        std::cout << "\n";
    }
}

void repositionCursor(const Buffer& buffer) {
    int row = buffer.get_cursor_row() + 4; 
    int col = buffer.get_cursor_col() + 5; 
    std::cout << "\x1b[" << row << ";" << col << "H";
    std::cout.flush();
}

int main(int argc, char* argv[]) {
    enableRawMode();
    atexit(disableRawMode);
    atexit(show_cursor); 

    hide_cursor(); // Hide sys cursor

    Buffer buffer;
    if (argc > 1) {
        if (!buffer.open(argv[1])) {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
    }

    bool running = true;
    Mode current_mode = NORMAL_MODE;

    while (running) {
        printBuffer(buffer);
        drawStatusBar(buffer, current_mode);
        repositionCursor(buffer);
        std::cout.flush();       

        if (current_mode == NORMAL_MODE) {
            NormalKeys key = parse_normal_key();

            switch (key) {
                case INSERT: current_mode = INSERT_MODE; break;
                case MOVE_UP: buffer.move_cursor(-1, 0); break;
                case MOVE_DOWN: buffer.move_cursor(1, 0); break;
                case MOVE_LEFT: buffer.move_cursor(0, -1); break;
                case MOVE_RIGHT: buffer.move_cursor(0, 1); break;
                case MOVE_NEXT_WORD: buffer.move_to_next_word(); break;
                case MOVE_PREVIOUS_WORD: buffer.move_to_previous_word(); break;
                case MOVE_END_OF_WORD: buffer.move_to_end_of_word(); break;
                case MOVE_BEGINNING_OF_LINE: buffer.move_to_beginning_of_line(); break;
                case MOVE_END_OF_LINE: buffer.move_to_end_of_line(); break;
                case DELETE: buffer.delete_char(); break;
                case SAVE: buffer.save(); break;
                case QUIT: running = false; break;
                case UNDO: /* TODO */ break;
                case REDO: /* TODO */ break;
                default: break;
            }
        } else { // INSERT_MODE
            char c;
            if (read(STDIN_FILENO, &c, 1) != 1) continue;

            if (c == 27) { // ESC
                current_mode = NORMAL_MODE;
            } else if (c == 13) { // Enter
                buffer.new_line();
            } else if (c == 127 || c == 8) { // Backspace
                buffer.move_cursor(0, -1);
                buffer.delete_char();
            } else {
                buffer.insert_char(c);
            }
        }
    }

    clearScreen();
    return 0;
}
