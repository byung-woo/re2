#include <iostream>

#include "re2/re2.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Need a regex string" << std::endl;
    return 1;
  }

  int64_t max_mem = 100 << 10;

  re2::RE2::Options opt;
  opt.set_max_mem(max_mem);
  opt.set_minimize_max_mem(true);

  re2::RE2 regex(argv[1], opt);
  
  if (!regex.ok()) {
    std::cerr << "Failed to create regex with \"" << argv[1] << "\"" << std::endl;
    std::cerr << " - error: " << regex.error() << std::endl;
    return 1;
  }

  std::cout << regex.MaxMem() << "\t" << argv[1] << std::endl;
  return 0;
}
