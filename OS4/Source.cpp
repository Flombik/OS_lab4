#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <iomanip>

#define MEMORY_SIZE 512
#define NO_SUCH_FILE 1
#define SUCCESS 0

using namespace std;

struct file {
	string name;
	string type;
	vector<int> indexes;
};

struct catalog {
	vector<file> files;
};

vector<string> memory(MEMORY_SIZE);
catalog root;

void dir() {
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		cout << it->name << '.' << it->type << endl;
	}
}

void createFile(string fileName, string fileType) {
	file newFile;
	newFile.name = fileName;
	newFile.type = fileType;
	root.files.push_back(newFile);
}

int deleteFile(string fileName, string fileType) {
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		if (it->name == fileName && it->type == fileType) {
			for (vector<int>::iterator index = it->indexes.begin(); index < it->indexes.end(); index++) {
				memory[*index].clear();
			}
			root.files.erase(it);
			return SUCCESS;
		}
	}
	return NO_SUCH_FILE;
}

int writeToFile(string fileName, string fileType, string data);
string readFromFile(string fileName, string fileType);

int copyFile(string fileName, string fileType) {
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		if (it->name == fileName && it->type == fileType) {
			file newFile;
			newFile.name = it->name + "(copy)";
			newFile.type = it->type;
			writeToFile(newFile.name, newFile.type, readFromFile(it->name, it->type));
			root.files.push_back(newFile);
			return SUCCESS;
		}
	}
	return NO_SUCH_FILE;
}

int renameFile(string fileName, string fileType, string newFileName) {
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		if (it->name == fileName && it->type == fileType) {
			it->name = newFileName;
			return SUCCESS;
		}
	}
	return NO_SUCH_FILE;
}

int writeToFile(string fileName, string fileType, string data) {
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		if (it->name == fileName && it->type == fileType) {
			for (unsigned int i = 0; i < data.size(); i++) {
				string temp;
				if (i += 7 < data.size()) {
					temp = data.substr(i - 1, 8);
					bool isWriten = false;
					for (unsigned int j = 0; j < MEMORY_SIZE; j++) {
						if (memory[j].empty() && !isWriten) {
							memory[j] = temp;
							it->indexes.push_back(j);
							isWriten = true;
						}
					}
				}
				else {
					temp = data.substr(i, data.size());
					bool isWriten = false;
					for (unsigned int j = 0; j < MEMORY_SIZE; j++) {
						if (memory[j].empty() && !isWriten) {
							memory[j] = temp;
							it->indexes.push_back(j);
							isWriten = true;
						}
					}
				}
				i += 6;
			}
			return SUCCESS;
		}
	}
	return NO_SUCH_FILE;
}

string readFromFile(string fileName, string fileType) {
	string temp = "";
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		if (it->name == fileName && it->type == fileType) {
			for (vector<int>::iterator index = it->indexes.begin(); index < it->indexes.end(); index++) {
				temp += memory[*index];
			}
			temp += '\n';
			return temp;
		}
	}
}

void createÑast(string dumpName) {
	ofstream out(dumpName, ios::binary | ios::out);
	unsigned int size = root.files.size();
	out.write((char*)&size, sizeof(size));
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		out.write((char*)&it->name, sizeof(it->name));
		out.write((char*)&it->type, sizeof(it->type));
		unsigned int indexesSize = it->indexes.size();
		out.write((char*)&indexesSize, sizeof(indexesSize));
		for (unsigned int i = 0; i < indexesSize; i++) {
			out.write((char*)&it->indexes[i], sizeof(it->indexes[i]));
		}
	}
	unsigned int memorySize = memory.size();
	out.write((char*)&memorySize, sizeof(memorySize));
	for (vector<string>::iterator it = memory.begin(); it < memory.end(); it++) {
		out.write((char*) & *it, sizeof(*it));
	}
	out.close();
}

void loadFromÑast(string dumpName) {
	ifstream in(dumpName, ios::binary | ios::in);
	unsigned int size;
	in.read((char*)&size, sizeof(size));
	for (unsigned int i = 0; i < size; i++) {
		file newFile;
		in.read((char*)&newFile.name, sizeof(newFile.name));
		in.read((char*)&newFile.type, sizeof(newFile.type));
		unsigned int indexesSize;
		in.read((char*)&indexesSize, sizeof(indexesSize));
		for (unsigned int i = 0; i < indexesSize; i++) {
			int temp;
			in.read((char*)&temp, sizeof(temp));
			newFile.indexes.push_back(temp);
		}
		root.files.push_back(newFile);
	}
	unsigned int memorySize;
	in.read((char*)&memorySize, sizeof(memorySize));
	for (int i = 0; i < memorySize; i++) {
		in.read((char*)&memory[i], sizeof(memory[i]));
	}
	in.close();
}

void dump(string dumpName) {
	ofstream dump(dumpName, ios::binary | ios::out);
	dump << "root>" << endl;
	for (vector<file>::iterator it = root.files.begin(); it < root.files.end(); it++) {
		dump << "	" << it->name << '.' << it->type << endl;
		dump << "	" << "----------------" << endl;
		dump << "	" << "| indexes      |" << endl;
		dump << "	" << "----------------" << endl;
		for (vector<int>::iterator index = it->indexes.begin(); index < it->indexes.end(); index++) {
			dump << "	" << "|     "; dump << setw(3) << *index; dump << "      |" << endl;
		}
		dump << "	" << "----------------" << endl;
		dump << "	" << "Info in file:\n	" << readFromFile(it->name, it->type) << endl;
		dump << endl;
	}
	dump << "-----------------------" << endl;
	dump << "|  Memory table state |" << endl;
	dump << "-----------------------" << endl;
	for (unsigned int i = 0; i < memory.size(); i++) {
		dump << "|"; dump << setw(10) << i; dump << "|"; dump << setw(10) << memory[i]; dump << "|" << endl;
		dump << "-----------------------" << endl;
	}
}

int main() {
	string request;
	while (true) {
		cout << "\n>";
		cin >> request;
		request = request.substr(0, request.find(' '));
		if (request == "cr") {
			string fileName_dot_Type;
			cin >> fileName_dot_Type;
			createFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size()));
		}
		else if (request == "rm") {
			string fileName_dot_Type;
			cin >> fileName_dot_Type;
			if (deleteFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size())) == NO_SUCH_FILE) {
				cout << "Can't delete file. No such file" << endl;
			}
		}
		else if (request == "dir") {
			dir();
		}
		else if (request == "write") {
			string fileName_dot_Type, input;
			cin >> fileName_dot_Type;
			cin >> input;
			//int first_qoute = input.find('\"');
			//int last_qoute = input.find('\"');
			if (writeToFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size()), input
			/*input.substr(first_qoute + 1, last_qoute - first_qoute)*/) == NO_SUCH_FILE) {
				cout << "Can't write to file. No such file" << endl;
			}
		}
		else if (request == "read") {
			string fileName_dot_Type;
			cin >> fileName_dot_Type;
			cout << readFromFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size()));
		}
		else if (request == "copy") {
			string fileName_dot_Type;
			cin >> fileName_dot_Type;
			if (copyFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size())) == NO_SUCH_FILE) {
				cout << "Can't copy file. No such file" << endl;
			}
		}
		else if (request == "move") {
			string fileName_dot_Type, newFileName;
			cin >> fileName_dot_Type;
			cin >> newFileName;
			if (renameFile(fileName_dot_Type.substr(0, fileName_dot_Type.find('.')),
				fileName_dot_Type.substr(fileName_dot_Type.find('.') + 1, fileName_dot_Type.size()),
				newFileName) == NO_SUCH_FILE) {
				cout << "Can't rename file. No such file" << endl;
			}
		}
		else if (request == "dump") {
			string dumpName;
			cin >> dumpName;
			dump(dumpName);
		}
		/*else if (request == "save") {
			string dumpName;
			cin >> dumpName;
			createÑast(dumpName);
		}
		else if (request == "load") {
			string dumpName;
			cin >> dumpName;
			loadFromÑast(dumpName);
		}*/
		else if (request == "help") {
			cout << "dir	Show all files in directory" << endl;
			cout << "cr	create file" << endl;
			cout << "rm	remove file" << endl;
			cout << "move	rename file" << endl;
			cout << "copy	copy file" << endl;
			cout << "write	write data to file" << endl;
			cout << "read	read data from file" << endl;
			/*cout << "save	save file system" << endl;
			cout << "load	load file system" << endl;*/
			cout << "dump	create dump" << endl;
			cout << "cls	clear screen" << endl;
			cout << "exit	finish program" << endl;
		}
		else if (request == "exit") {
			return 0;
		}
		else if (request == "cls") {
			system("cls");
		}
		else {
			cout << "Unknown command..." << endl;
		}
	}
}