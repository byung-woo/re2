#include <iostream>
#include <string>

#include "re2/re2.h"

int main(int argc, char* argv[]) {
  int64_t max_mem = 1 << 20;

  std::vector<std::string> patterns;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
      std::cout << "Usage: re2_minimized_mem_budget [OPTIONS]\n"
                << "Get minimized memory budget for each regex pattern\n"
                << "Example: re2_minimized_mem_budget -p '(foo|bar)' -p '[a-z]*'\n"
                << "         re2_minimized_mem_budget < patterns.txt\n"
                << "\n"
                << "  -p PATTERN    regular expression pattern string\n"
                << "\n"
                << "If no patterns are specified, then the standard input is read.\n";
      return 0;
    }
    else if (strcmp(argv[i], "-p") == 0) {
      if (++i >= argc) {
        std::cerr << "Need to specify regex pattern." << std::endl;
        std::cerr << " # re2_memprof -p REGEX_PATTERN" << std::endl;
        return 1;
      }
      patterns.push_back(argv[i]);

    } else {
        std::cerr << "Invalid argument: " << argv[i] << std::endl;
        return 1;
    }
  }

  if (patterns.size() == 0) {
    for (std::string line; std::getline(std::cin, line);) {
      if (line.empty())
        break;
      patterns.push_back(line);
    }
  }

  if (patterns.size() == 0) {
    std::cerr << "Need regex patterns." << std::endl;
    std::cerr << " # re2_minimized_mem_budget -p REGEX_PATTERN" << std::endl;
    std::cerr << " # re2_minimized_mem_budget < patterns.txt" << std::endl;
    return 1;
  }

  re2::RE2::Options opt;
  opt.set_max_mem(max_mem);
  opt.set_minimize_mem_budget(true);

  std::cout << "# regex_pattern" << "\t" << "mem_budget" << std::endl;

  for (std::string pattern: patterns) {
    re2::RE2 regex(pattern, opt);

    if (!regex.ok()) {
      std::cerr << "Failed to create regex with \"" << pattern << "\"" << std::endl;
      std::cerr << " - error: " << regex.error() << std::endl;
      return 1;
    }

    std::cout << pattern << "\t" << regex.MemBudget() << std::endl;
  }
  return 0;
}
