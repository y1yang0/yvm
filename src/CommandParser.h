#ifndef YVM_COMMANDPARSER_H
#define YVM_COMMANDPARSER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
struct Flag {
	Flag(std::string SimpleName, std::string FullName = "", std::string ExtraInformation = "") {
		this->SimpleName       = SimpleName;
		this->FullName         = FullName;
		this->ExtraInforamtion = ExtraInformation;
	}
	void updateAttribute(std::string attribute) {
		attributes.push_back(attribute);
	}
	void getAttributes() {
		std::cout << "|SimpleName :" << SimpleName << "|FullName :" << FullName << "|ExtraInformation :" << ExtraInforamtion << " |The attributes of  " << FullName << " is : ";
		for (auto& s : attributes) {
			std::cout << s << " ";
		}
		std::cout << std::endl;
	}

	inline std::vector<std::string>& getFlagValues() { return attributes; }

private:
	std::string SimpleName;
	std::string FullName;
	std::string ExtraInforamtion;
	std::vector<std::string> attributes;
};
struct Parser {
	Parser() {}
	void parse(int argc, char** argv) {
		for (int i = 1; i < argc; i++) {
			parseHelper(argv[i]);
		}
	}
	void parseHelper(std::string objString) {
		std::string temp_string = objString;
		if (temp_string.substr(0, 2) == "--") {
			temp_string.erase(0, 2);
			size_t pos = temp_string.find_first_of('=');
			if (pos != string::npos) {
				std::string theFlag = temp_string.substr(0, pos);
				temp_string.erase(0, pos + 1);
				while (temp_string.size() != 0) {
					size_t attributePos = temp_string.find_first_of(';');
					if (attributePos == string::npos) {
						Flags[FullNames[theFlag]].updateAttribute(temp_string);
						temp_string.clear();
					} else {
						Flags[FullNames[theFlag]].updateAttribute(temp_string.substr(0, attributePos));
						temp_string.erase(0, attributePos + 1);
					}
				}
			}
		} else if (temp_string.substr(0, 1) == "-") {
			temp_string.erase(0, 1);
			if (temp_string[0] == 'h') {
				needHelpFlag = true;
			}
			size_t pos = temp_string.find_first_of('=');
			if (pos != string::npos) {
				std::string theFlag = temp_string.substr(0, pos);
				temp_string.erase(0, pos + 1);
				while (temp_string.size() != 0) {
					size_t attributePos = temp_string.find_first_of(';');
					if (attributePos == string::npos) {
						Flags[SimpleNames[theFlag]].updateAttribute(temp_string);
					} else {
						Flags[SimpleNames[theFlag]].updateAttribute(temp_string.substr(0, attributePos));
						temp_string.erase(0, attributePos + 1);
					}
				}
			}
		} else {
			runningProgram = temp_string;
		}
	}

	void addFlag(std::string SimpleName, std::string FullName, std::string ExtraInformation) {
		theIndex++;
		SimpleNames.insert(std::make_pair(SimpleName, theIndex));
		FullNames.insert(std::make_pair(FullName, theIndex));
		Flag the_flag(SimpleName, FullName, ExtraInformation);
		Flags.push_back(the_flag);
	}
	bool hasSimpleName(std::string objString) {
		bool mark = false;
		if (SimpleNames.find(objString) != SimpleNames.end()) {
			mark = true;
		}
		return mark;
	}
	bool hasFullName(std::string objString) {
		bool mark = false;
		if (FullNames.find(objString) != FullNames.end()) {
			mark = true;
		}
		return mark;
	}
	void getFlags() {
		for (auto& s : Flags) {
			s.getAttributes();
		}
	}

	inline std::string& getRunningProgram() { return runningProgram; }

	inline std::vector<std::string>& getFlagByName(const std::string& str) {
		return Flags.at(SimpleNames.find(str)->second).getFlagValues();
	}

	inline bool needHelp() const { return needHelpFlag; }

private:
	std::string runningProgram;
	int theIndex = -1;
	// Whats this shit...
	std::map<std::string, int> SimpleNames;
	std::map<std::string, int> FullNames;
	std::vector<Flag> Flags;

	bool needHelpFlag = false;
};
#endif