#include "keymaps.hpp"
#include "buffer.hpp"

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>

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
}

void printBuffer(const Buffer& buffer) {
    clearScreen();
    
    std::cout << "Ryu Editor - " << (buffer.get_filename().empty() ? "[No Name]" : buffer.get_filename())
              << (buffer.is_modified() ? " [+]" : "") << "\n";
    std::cout << "--------------------\n";
    
    const auto& content = buffer.get_content();
    int cursor_row = buffer.get_cursor_row();
    int cursor_col = buffer.get_cursor_col();
    
    for (size_t i = 0; i < content.size(); ++i) {
        const std::string& line = content[i];
        
        for (size_t j = 0; j < line.length(); ++j) {
            if (i == cursor_row && j == cursor_col) {
                std::cout << "\x1b[7m"; 
                std::cout << (line[j] == '\0' ? ' ' : line[j]); 
                std::cout << "\x1b[0m";
            } else {
                std::cout << line[j];
            }
        }
        
        if (i == cursor_row && cursor_col >= line.length()) {
            std::cout << "\x1b[7m "; 
            std::cout << "\x1b[0m";
        }
        
        std::cout << "\n";
    }
    
    std::cout << "\x1b[23;0H";  
    std::cout << "-- " << (cursor_row + 1) << "," << (cursor_col + 1) << " --";
    
    std::cout << "\x1b[24;0H";
    std::cout << "-- " << (buffer.get_cursor_row() + 1) << "," << (buffer.get_cursor_col() + 1) << " --";
    
    int row = cursor_row + 3;  
    int col = cursor_col + 1;  
    std::cout << "\x1b[" << row << ";" << col << "H";
    std::cout.flush();
}

const char* mode_to_string(Mode mode) {
    switch (mode) {
        case NORMAL_MODE: return "NORMAL";
        case INSERT_MODE: return "INSERT";
        default: return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) {
    // Set up terminal
    enableRawMode();
    atexit(disableRawMode);
    
    Buffer buffer;
    
    if (argc > 1) {
        if (!buffer.open(argv[1])) {
            disableRawMode();
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    bool running = true;
    Mode current_mode = NORMAL_MODE;
    
    printBuffer(buffer);
    
    while (running) {
        std::cout << "\x1b[24;0H";  
        std::cout << "-- " << mode_to_string(current_mode) << " --     ";
        std::cout.flush();
        
        int row = buffer.get_cursor_row() + 3;
        int col = buffer.get_cursor_col() + 1;
        std::cout << "\x1b[" << row << ";" << col << "H";
        std::cout.flush();
        
        if (current_mode == NORMAL_MODE) {
            NormalKeys key = parse_normal_key();
            
            switch (key) {
                case INSERT:
                    current_mode = INSERT_MODE;
                    break;
                case MOVE_UP:
                    buffer.move_cursor(-1, 0);
                    break;
                case MOVE_DOWN:
                    buffer.move_cursor(1, 0);
                    break;
                case MOVE_LEFT:
                    buffer.move_cursor(0, -1);
                    break;
                case MOVE_RIGHT:
                    buffer.move_cursor(0, 1);
                    break;
                case MOVE_NEXT_WORD:
                    buffer.move_to_next_word();
                    break;
                case MOVE_PREVIOUS_WORD:
                    buffer.move_to_previous_word();
                    break;
                case MOVE_END_OF_WORD:
                    buffer.move_to_end_of_word();
                    break;
                case MOVE_BEGINNING_OF_LINE:
                    buffer.move_to_beginning_of_line();
                    break;
                case MOVE_END_OF_LINE:
                    buffer.move_to_end_of_line();
                    break;
                case DELETE:
                    buffer.delete_char();
                    break;
                case SAVE:
                    buffer.save();
                    break;
                case QUIT:
                    running = false;
                    break;
                case UNDO:
                    // Undo functionality to be implemented
                    break;
                case REDO:
                    // Redo functionality to be implemented
                    break;
                case -1:
                    break;
                default:
                    break;
            }
        }
        else {
            char c;
            if (read(STDIN_FILENO, &c, 1) != 1) continue;
            
            if (c == 27) {  
                current_mode = NORMAL_MODE;
            }
            else if (c == 13) {  
                buffer.new_line();
            }
            else if (c == 127 || c == 8) {  
                buffer.move_cursor(0, -1);
                buffer.delete_char();
            }
            else {
                buffer.insert_char(c);
            }
        }
        
        printBuffer(buffer);
    }
    
    return 0;
}
