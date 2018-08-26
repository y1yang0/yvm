#include "CommandParser.h"
#include "Option.h"
#include "YVM.h"

int main(int argc, char* argv[]) {
	Parser cparser;
	cparser.addFlag("h", "help", "List help documentations and uasges.");
	cparser.addFlag("rt", "runtime", "Attach java runtime libraries of this YVM.");
	cparser.addFlag("sp", "searchpath", "Add *.class to search path.");
	cparser.parse(argc, argv);

	bool needHelp = cparser.needHelp();
	if (needHelp) {
		std::cout << "Yvm - Yet another java Virtual Machine :)\n"
		             "Usage:\n"
		             "./yvm [option|option=value] program_name\n\n"
		             "option:\n"
		             "-h or --help\t\t\tList help documentations and usages.\n"
		             "-rt or --runtime(required)\t\t\tAttach java runtime "
		             "libraries of this YVM.\n"
		             "-sp or --searchpath\t\t\t Add *.class to search path.\n\n"
		             "You must specify the \"runtime\" flag to tell yvm where "
		             "it "
		             "could find jdk classes, and also program name is "
		             "required.\n";
		return 0;
	}
	auto& runtimeLibs = cparser.getFlagByName("rt");
	auto& searchLibs  = cparser.getFlagByName("sp");
	runtimeLibs.insert(runtimeLibs.end(), searchLibs.begin(), searchLibs.end());

	YVM vm;
	vm.warmUp(runtimeLibs);
	auto& runningProgram = cparser.getRunningProgram();
	if (runningProgram.empty()) {
		std::cerr << " Fatal error: no running program specified.\n";
		return 1;
	}
	for (auto& c : runningProgram) {
		if (c == '.') {
			c = '/';
		}
	}
	vm.callMain(cparser.getRunningProgram().c_str());
	return 0;
}