#include <boost/program_options.hpp>
#include <iostream>
#include "../misc/Option.h"
#include "YVM.h"

int main(int argc, char* argv[]) {
    using namespace boost::program_options;

    // Command arguments handling
    options_description opts("Usage");
    opts.add_options()("help", "List help documentations and usages.")(
        "runtime", value<std::vector<std::string>>(),
        "Attach java runtime libraries where yvm would lookup classes at")(
        "run", value<std::string>(), "Program which would be executed soon");
    positional_options_description p;
    p.add("run", -1);

    variables_map vm;
    store(command_line_parser(argc, argv).options(opts).positional(p).run(),
          vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << opts
                  << "You must specify the \"runtime\" flag to tell yvm where "
                     "it could find jdk classes, and also program name is "
                     "required.\n";
        return 0;
    }
    if (!vm.count("run")) {
        std::cerr << "Fatal error: no running program specified.\n";
        return 1;
    }
    if (!vm.count("runtime")) {
        std::cerr << "Fatal error: no runtime attached into yvm\n";
        return 1;
    }

    // Create virtual machine and executing code
    YVM yvm;
    yvm.warmUp(vm["runtime"].as<std::vector<std::string>>());
    std::string internalUsedClassName{vm["run"].as<std::string>()};
    for (auto& c : internalUsedClassName) {
        if (c == '.') {
            c = '/';
        }
    }
    yvm.callMain(internalUsedClassName);
    return 0;
}