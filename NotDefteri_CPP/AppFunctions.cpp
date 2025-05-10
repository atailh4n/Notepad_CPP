/*
* Copyright 2025, Ata Ilhan Kokturk and Aleksander Płomiński
* Licensed under the MIT License.
* See LICENSE file for detailed information.
* Modified by Aleksander Płomiński
*/
#include <iostream>
#include <limits>
#include <unordered_map>
#include <fstream>
#include <cstdlib>

#include "Note.h"
#include "NoteStruct.h"
#include "AppFunctions.h"

void ClearScreen() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        std::cout << "\033[2J\033[1;1H";  // Linux, macOS
    #endif
}
void AppEntry() {
	ShowInfo();
	HandleInput();
}

void ShowInfo() {
	std::cout << R"(
CPPNote Program
Copyright 2025 - Ata Ilhan Kokturk, Aleksander Płomiński

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
			ClearScreen();
			std::cout << "Invalid option. Please choose:\n9 - Return to main menu\n5 - Exit\n";
			continue;
		}

		if (choice == 9) {
			ClearScreen();
			AppEntry();
		}
		else {
			exit(0);
		}
	}
}

void CreateNote() {
	ClearScreen();

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
	ClearScreen();
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
	ClearScreen();

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
		if (line.starts_with("TITLE: ")) note.title = line.substr(7);
		else if (line.starts_with("AUTHOR: ")) note.author = line.substr(8);
		else if (line.starts_with("NDATA: ")) note.content = line.substr(7);
	}

	ClearScreen();
	std::cout << "Note Preview:\n--------------------\n";
	std::cout << "Title: " << note.title << "\n";
	std::cout << "Author: " << note.author << "\n";
	std::cout << "Content: " << note.content << "\n";

	ShowMenu();
}

void DeleteNote() {
	ClearScreen();

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
			ClearScreen();
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
			ClearScreen();
			ShowInfo();
			break;
		}
	}
}
