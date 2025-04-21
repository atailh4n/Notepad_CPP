/*
* Copyright 2025, Ata Ilhan Kokturk
* Has MIT License
* See LICENSE file for detailed information.
*/

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <limits>
#include <conio.h>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>

namespace fs = std::filesystem;

class NoteStruct {
public:
	std::string title;
	std::string author;
	long long date;
	std::string note_data;

	NoteStruct(std::string title, std::string author, long long date, std::string note_data)
		: title(std::move(title)), author(std::move(author)), date(date), note_data(std::move(note_data)) {

		std::cout << "Creating new note..." << std::endl;

		fs::path notesDir = fs::current_path() / "Notes";
		if (!fs::is_directory(notesDir)) {
			fs::create_directory(notesDir);
		}

		std::string fileNameStr = SanitizeFileName(this->title) + "_" + std::to_string(date) + ".cppndat";
		fs::path filePath = fileNameStr;

		FILE* fp;
		errno_t err = fopen_s(&fp, filePath.string().c_str(), "w");

		if (err) {
			char errBuff[256];
			strerror_s(errBuff, err);
			std::cerr << "Cannot open file '" << filePath << "': " << errBuff << std::endl;
		}
		else {
			WriteField(fp, "TITLE: ", this->title, true);
			WriteField(fp, "AUTHOR: ", this->author, true);
			WriteField(fp, "NDATA: ", this->note_data, false);
			fclose(fp);
		}

		fs::copy_file(filePath, notesDir / fileNameStr, fs::copy_options::overwrite_existing);

		std::cout << "Note creation successfull." << std::endl;
	}

private:
	void WriteField(FILE* fp, const std::string& prefix, const std::string& content, bool newline) {
		fwrite(prefix.c_str(), 1, prefix.length(), fp);
		fwrite(content.c_str(), 1, content.length(), fp);
		if (newline) fputc('\n', fp);
	}

	std::string SanitizeFileName(const std::string& input) {
		std::string result = input;
		for (char& c : result) {
			if (strchr("<>:\"/\\|?* ", c)) c = '-';
		}
		return result;
	}
};

struct Note {
	std::string title;
	std::string author;
	std::string content;
};

// Function declarations
void AppEntry();
void ShowInfo();
void ShowMenu();
void CreateNote();
void ListNotes();
void EditNote();
void DeleteNote();
void HandleInput();

void AppEntry() {
	ShowInfo();
	HandleInput();
}

void ShowInfo() {
	std::cout << R"(
CPPNote Program
Copyright 2025 - Ata Ilhan Kokturk

Please select one of the modes below:
------------------------------------
1 - Create new note
2 - List all notes
3 - Edit note
4 - Delete note
5 - Exit
------------------------------------
> )";
}

void ShowMenu() {
	unsigned short choice = 0;
	std::cout << "\n------------------------------------\n";
	std::cout << "9 - Return to main menu\n";
	std::cout << "5 - Exit\n";

	while (true) {
		std::cin >> choice;
		if (std::cin.fail() || (choice != 9 && choice != 5)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			system("cls");
			std::cout << "Invalid option. Please choose:\n9 - Return to main menu\n5 - Exit\n";
			continue;
		}

		if (choice == 9) {
			system("cls");
			AppEntry();
		}
		else {
			exit(0);
		}
	}
}

void CreateNote() {
	system("cls");

	std::string title = "Untitled";
	std::string author = "Anonymous";
	std::string content;

	time_t currentTime = time(nullptr);
	long long date = static_cast<long long>(currentTime);

	std::cout << "Enter a title:\n> ";
	// std::numeric_limits<std::streamsize>::max(), '\n'
	// Basically this does include the first char too because cin.ignore deletes first char.
	// Learned from hard way again.
	// Total Wasted Time: ~15min
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, title);

	std::cout << "Enter author name:\n> ";
	std::getline(std::cin, author);

	std::cout << "Enter your note:\n> ";
	std::getline(std::cin, content);

	NoteStruct(title, author, date, content);
	ShowMenu();
}

void ListNotes() {
	system("cls");
	fs::path notesPath = fs::current_path() / "Notes";

	if (!fs::exists(notesPath)) {
		std::cerr << "Notes folder not found: " << notesPath << "\n";
		ShowMenu();
		return;
	}

	std::cout << "Your notes:\n------------------------------------\n";
	unsigned int index = 1;
	for (const auto& entry : fs::directory_iterator(notesPath)) {
		if (entry.path().extension() == ".cppndat") {
			std::cout << index++ << " - " << entry.path().filename() << "\n";
		}
	}
	ShowMenu();
}

void EditNote() {
	system("cls");

	fs::path notesPath = fs::current_path() / "Notes";
	if (!fs::exists(notesPath)) {
		std::cerr << "Notes folder not found: " << notesPath << "\n";
		ShowMenu();
		return;
	}

	std::unordered_map<unsigned int, std::string> fileMap;
	unsigned int index = 1;

	for (const auto& entry : fs::directory_iterator(notesPath)) {
		if (entry.path().extension() == ".cppndat") {
			std::cout << index << " - " << entry.path().filename() << "\n";
			fileMap[index++] = entry.path().filename().string();
		}
	}

	std::cout << "Enter the number of the note to view/edit:\n> ";
	unsigned int selection = 0;
	std::cin >> selection;

	if (fileMap.find(selection) == fileMap.end()) {
		std::cout << "Invalid selection.\n";
		ShowMenu();
		return;
	}

	Note note;
	std::ifstream file(notesPath / fileMap[selection]);
	std::string line;

	while (std::getline(file, line)) {
		if (line._Starts_with("TITLE: ")) note.title = line.substr(7);
		else if (line._Starts_with("AUTHOR: ")) note.author = line.substr(8);
		else if (line._Starts_with("NDATA: ")) note.content = line.substr(7);
	}

	system("cls");
	std::cout << "Note Preview:\n--------------------\n";
	std::cout << "Title: " << note.title << "\n";
	std::cout << "Author: " << note.author << "\n";
	std::cout << "Content: " << note.content << "\n";

	ShowMenu();
}

void DeleteNote() {
	system("cls");

	fs::path notesPath = fs::current_path() / "Notes";
	if (!fs::exists(notesPath)) {
		std::cerr << "Notes folder not found: " << notesPath << "\n";
		ShowMenu();
		return;
	}

	std::unordered_map<unsigned int, std::string> fileMap;
	unsigned int index = 1;

	for (const auto& entry : fs::directory_iterator(notesPath)) {
		if (entry.path().extension() == ".cppndat") {
			std::cout << index << " - " << entry.path().filename() << "\n";
			fileMap[index++] = entry.path().filename().string();
		}
	}

	std::cout << "Enter the number of the note to delete:\n> ";
	unsigned int selection = 0;
	std::cin >> selection;

	if (fileMap.find(selection) == fileMap.end()) {
		std::cout << "Invalid selection.\n";
		ShowMenu();
		return;
	}

	std::cout << "Are you sure you want to delete this note? (Y/N):\n> ";
	char confirm;
	std::cin >> confirm;

	if (toupper(confirm) == 'Y') {
		fs::remove(notesPath / fileMap[selection]);
		std::cout << "Note deleted.\n";
	}
	else {
		std::cout << "Deletion canceled.\n";
	}

	ShowMenu();
}

void HandleInput() {
	unsigned short mode = 0;

	while (true) {
		std::cin >> mode;
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			system("cls");
			ShowInfo();
			continue;
		}

		switch (mode) {
		case 1: CreateNote(); return;
		case 2: ListNotes(); return;
		case 3: EditNote(); return;
		case 4: DeleteNote(); return;
		case 5: exit(0);
		default:
			system("cls");
			ShowInfo();
			break;
		}
	}
}

int main() {
	AppEntry();
}
