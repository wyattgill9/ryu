#include "buffer.hpp"
#include <iostream>
#include <cctype>

Buffer::Buffer() : modified(false), cursor_row(0), cursor_col(0) {
    lines.push_back("");
}

Buffer::Buffer(const std::string& filename) : modified(false), cursor_row(0), cursor_col(0) {
    if (!open(filename)) {
        lines.push_back("");
    }
}

bool Buffer::open(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    lines.clear();
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    if (lines.empty()) {
        lines.push_back("");
    }
    
    this->filename = filename;
    modified = false;
    cursor_row = 0;
    cursor_col = 0;
    
    return true;
}

bool Buffer::save() {
    if (filename.empty()) {
        return false;
    }
    return save_as(filename);
}

bool Buffer::save_as(const std::string& new_filename) {
    std::ofstream file(new_filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (size_t i = 0; i < lines.size(); ++i) {
        file << lines[i];
        if (i < lines.size() - 1) {
            file << '\n';
        }
    }
    
    filename = new_filename;
    modified = false;
    return true;
}

void Buffer::insert_char(char c) {
    if (cursor_row >= lines.size()) {
        cursor_row = lines.size() - 1;
    }
    
    if (cursor_col > lines[cursor_row].length()) {
        cursor_col = lines[cursor_row].length();
    }
    
    lines[cursor_row].insert(cursor_col, 1, c);
    cursor_col++;
    modified = true;
}

void Buffer::delete_char() {
    if (cursor_row >= lines.size()) {
        return;
    }
    
    if (cursor_col < lines[cursor_row].length()) {
        lines[cursor_row].erase(cursor_col, 1);
        modified = true;
    } 
    else if (cursor_row < lines.size() - 1) {
        lines[cursor_row] += lines[cursor_row + 1];
        lines.erase(lines.begin() + cursor_row + 1);
        modified = true;
    }
}

void Buffer::new_line() {
    if (cursor_row >= lines.size()) {
        cursor_row = lines.size() - 1;
    }
    
    std::string current_line = lines[cursor_row];
    std::string new_line;
    
    if (cursor_col < current_line.length()) {
        new_line = current_line.substr(cursor_col);
        lines[cursor_row] = current_line.substr(0, cursor_col);
    }
    
    lines.insert(lines.begin() + cursor_row + 1, new_line);
    cursor_row++;
    cursor_col = 0;
    modified = true;
}

void Buffer::move_cursor(int row_delta, int col_delta) {
    cursor_row += row_delta;
    cursor_col += col_delta;
    
    if (cursor_row < 0) cursor_row = 0;
    if (cursor_row >= lines.size()) cursor_row = lines.size() - 1;
    
    if (cursor_col < 0) cursor_col = 0;
    if (cursor_col > lines[cursor_row].length()) cursor_col = lines[cursor_row].length();
}

void Buffer::set_cursor(int row, int col) {
    cursor_row = row;
    cursor_col = col;
    
    if (cursor_row < 0) cursor_row = 0;
    if (cursor_row >= lines.size()) cursor_row = lines.size() - 1;
    
    if (cursor_col < 0) cursor_col = 0;
    if (cursor_col > lines[cursor_row].length()) cursor_col = lines[cursor_row].length();
}

void Buffer::move_to_beginning_of_line() {
    cursor_col = 0;
}

void Buffer::move_to_end_of_line() {
    if (cursor_row < lines.size()) {
        cursor_col = lines[cursor_row].length();
    }
}

void Buffer::move_to_next_word() {
    if (cursor_row >= lines.size()) return;
    
    const std::string& line = lines[cursor_row];
    size_t pos = cursor_col;
    
    if (pos >= line.length()) {
        if (cursor_row < lines.size() - 1) {
            cursor_row++;
            cursor_col = 0;
        }
        return;
    }
    
    while (pos < line.length() && !std::isspace(line[pos])) {
        pos++;
    }
    
    // Skip spaces
    while (pos < line.length() && std::isspace(line[pos])) {
        pos++;
    }
    
    if (pos >= line.length()) {
        if (cursor_row < lines.size() - 1) {
            cursor_row++;
            cursor_col = 0;
        } else {
            cursor_col = line.length();
        }
    } else {
        cursor_col = pos;
    }
}

void Buffer::move_to_previous_word() {
    if (cursor_row >= lines.size()) return;
    
    const std::string& line = lines[cursor_row];
    int pos = cursor_col;
    
    if (pos <= 0) {
        if (cursor_row > 0) {
            cursor_row--;
            cursor_col = lines[cursor_row].length();
        }
        return;
    }
    
    pos--;
    
    while (pos >= 0 && std::isspace(line[pos])) {
        pos--;
    }
    
    while (pos >= 0 && !std::isspace(line[pos])) {
        pos--;
    }
    
    cursor_col = (pos < 0) ? 0 : pos + 1;
}

void Buffer::move_to_end_of_word() {
    if (cursor_row >= lines.size()) return;
    
    const std::string& line = lines[cursor_row];
    size_t pos = cursor_col;
    
    if (pos >= line.length()) {
        if (cursor_row < lines.size() - 1) {
            cursor_row++;
            cursor_col = 0;
        }
        return;
    }
    
    while (pos < line.length() && std::isspace(line[pos])) {
        pos++;
    }
    
    while (pos < line.length() && !std::isspace(line[pos])) {
        pos++;
    }
    
    cursor_col = (pos == 0) ? 0 : pos - 1;
}
