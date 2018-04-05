#include<iostream>
#include<string>
#include<map>
#include<vector>
using namespace std;
struct Flag
{
	Flag(std::string SimpleName,std::string FullName="",std::string ExtraInformation="") {
		this->SimpleName = SimpleName;
		this->FullName = FullName;
		this->ExtraInforamtion = ExtraInformation;
	}
	void updateAttribute(std::string attribute) {
		attributes.push_back(attribute);
	}
	void getAttributes() {
		std::cout << "|SimpleName :" << SimpleName << "|FullName :" << FullName << "|ExtraInformation :" << ExtraInforamtion << " |The attributes of  " << FullName << " is : ";
		for (auto &s : attributes) {
			std::cout << s << " ";
		}
		std::cout << std::endl;
	}
private:
	std::string SimpleName;
	std::string FullName;
	std::string ExtraInforamtion;
	std::vector<std::string> attributes;

};
struct Parser 
{
	Parser() {
	}
	void Parse(int argc, char ** argv) {
	commandName= argv[1];
		for (int i = 2; i < argc; i++)
		{
			parseHelper(argv[i]);
		}
	}
	void parseHelper(std::string objString) {
		std::string temp_string = objString;
		if (temp_string.substr(0,2)=="--")
		{
			temp_string.erase(0, 2);
			size_t pos= temp_string.find_first_of('=');
			if (pos != string::npos) {
			  std::string theFlag= temp_string.substr(0, pos);
			  temp_string.erase(0, pos+1);
			  while (temp_string.size()!=0)
			  {
				  size_t attributePos = temp_string.find_first_of(';');
				  if (attributePos == string::npos) {
					  Flags[FullNames[theFlag]].updateAttribute(temp_string);
					  temp_string.clear();
				  }
				  else
				  {
					  Flags[FullNames[theFlag]].updateAttribute(temp_string.substr(0,attributePos));
					  temp_string.erase(0, attributePos + 1);
				  }	 
			  }
			}
		}
		else if (temp_string.substr(0, 1) == "-") {

			temp_string.erase(0, 1);
			size_t pos = temp_string.find_first_of('=');
			if (pos != string::npos) {
				std::string theFlag = temp_string.substr(0, pos);
				temp_string.erase(0, pos + 1);
				while (temp_string.size() != 0)
				{
					size_t attributePos = temp_string.find_first_of(';');
					if (attributePos == string::npos) {
						Flags[SimpleNames[theFlag]].updateAttribute(temp_string);
					}
					else
					{
						Flags[SimpleNames[theFlag]].updateAttribute(temp_string.substr(0, attributePos));
						temp_string.erase(0, attributePos + 1);
					}
				}
			}
		}
	}


	void addFlag(std::string SimpleName, std::string FullName, std::string ExtraInformation){
		theIndex++;
		SimpleNames.insert(std::make_pair(SimpleName,theIndex));
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
		for (auto &s : Flags) {
			s.getAttributes();
		}
	}
//private:
	int theIndex = -1;
	std::string commandName;
	std::map<std::string, int> SimpleNames;
	std::map<std::string, int> FullNames;
	std::vector<Flag> Flags;
};

void main(int argc,char **argv) {
	std::cout << argc << " " << std::endl;
	Parser *aParse = new Parser();
	aParse->addFlag("h", "help", "this is the operation help");
	aParse->addFlag("d", "debug", "the inforamation debug");
	aParse->addFlag("w", "wocao", "just a Wocao ");
	aParse->Parse(argc, argv);
	std::cout << aParse->commandName << std::endl;
	aParse->getFlags();
	
	

}