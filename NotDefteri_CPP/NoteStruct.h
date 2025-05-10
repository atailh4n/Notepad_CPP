/*
* Copyright 2025, Ata Ilhan Kokturk and Aleksander Płomiński
* Licensed under the MIT License.
* See LICENSE file for detailed information.
* Modified by Aleksander Płomiński
*/
#ifndef NOTESTRUCT_H
#define NOTESTRUCT_H

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class NoteStruct {
public:
    std::string title;
    std::string author;
    long long date;
    std::string note_data;

    NoteStruct(std::string title, std::string author, long long date, std::string note_data);

private:
    void WriteField(FILE* fp, const std::string& prefix, const std::string& content, bool newline);
    std::string SanitizeFileName(const std::string& input);
};

#endif // NOTESTRUCT_H
