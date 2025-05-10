/*
* Copyright 2025, Ata Ilhan Kokturk and Aleksander Płomiński
* Licensed under the MIT License.
* See LICENSE file for detailed information.
* Modified by Aleksander Płomiński
*/
#include <iostream>
#include "NoteStruct.h"
#include <cstring>

NoteStruct::NoteStruct(std::string title, std::string author, long long date, std::string note_data)
    : title(std::move(title)), author(std::move(author)), date(date), note_data(std::move(note_data)) {

    std::cout << "Creating new note..." << std::endl;

    fs::path notesDir = fs::current_path() / "Notes";
    if (!fs::is_directory(notesDir)) {
        fs::create_directory(notesDir);
    }

    std::string fileNameStr = SanitizeFileName(this->title) + "_" + std::to_string(date) + ".cppndat";
    fs::path filePath = fileNameStr;

    FILE* fp = fopen(filePath.string().c_str(), "w");

    if (!fp) {
        std::cerr << "Cannot open file '" << filePath << "': " << strerror(errno) << std::endl;
        //stop this function if error detected.
        return;
    } 
    else {
        WriteField(fp, "TITLE: ", this->title, true);
        WriteField(fp, "AUTHOR: ", this->author, true);
        WriteField(fp, "NDATA: ", this->note_data, false);
        fclose(fp);
    }

    fs::copy_file(filePath, notesDir / fileNameStr, fs::copy_options::overwrite_existing);

    std::cout << "Note creation successful." << std::endl;
}

void NoteStruct::WriteField(FILE* fp, const std::string& prefix, const std::string& content, bool newline) {
    fwrite(prefix.c_str(), 1, prefix.length(), fp);
    fwrite(content.c_str(), 1, content.length(), fp);
    if (newline) fputc('\n', fp);
}

std::string NoteStruct::SanitizeFileName(const std::string& input) {
    std::string result = input;
    for (char& c : result) {
        if (strchr("<>:\"/\\|?* ", c)) c = '-';
    }
    return result;
}
