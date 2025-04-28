#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string>
#include <vector>
#include <fstream>

class Buffer {
private:
    std::vector<std::string> lines;
    std::string filename;
    bool modified;
    int cursor_row;
    int cursor_col;

public:
    Buffer();
    explicit Buffer(const std::string& filename);
    
    bool open(const std::string& filename);
    bool save();
    bool save_as(const std::string& new_filename);
    
    void insert_char(char c);
    void delete_char();
    void new_line();
    
    void move_cursor(int row_delta, int col_delta);
    void set_cursor(int row, int col);
    void move_to_beginning_of_line();
    void move_to_end_of_line();
    void move_to_next_word();
    void move_to_previous_word();
    void move_to_end_of_word();
    
    int get_cursor_row() const { return cursor_row; }
    int get_cursor_col() const { return cursor_col; }
    const std::vector<std::string>& get_content() const { return lines; }
    std::string get_filename() const { return filename; }
    bool is_modified() const { return modified; }
};

#endif
