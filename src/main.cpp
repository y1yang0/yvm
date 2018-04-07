#include "YVM.h"
#include "CommandParser.h"
#include "Option.h"

int main(int argc, char* argv[]) {
    Parser cparser;
    cparser.addFlag("h", "help", "List help documentations and uasges.");
    cparser.addFlag("rt", "runtime", "Attach java runtime libraries of this YVM.");
    cparser.addFlag("sp", "searchpath", "Add *.class searching path.");
    cparser.parse(argc, argv);
    
    auto & runtimeLibs = cparser.getFlagByName("rt");
    auto & searchLibs = cparser.getFlagByName("sp");
    runtimeLibs.insert(runtimeLibs.end(), searchLibs.begin(), searchLibs.end());

    YVM vm;
    vm.warmUp(runtimeLibs);
    auto & runningProgram = cparser.getRunningProgram();
    if(runningProgram.empty()) {
        std::cerr << " Fatal error: no running program specified.\n";
        return 1;
    }
    vm.callMain(cparser.getRunningProgram().c_str());
#ifdef YVM_DEBUG_SUSPEND_CONSOLE
    system("pause");
#endif
    return 0;
}