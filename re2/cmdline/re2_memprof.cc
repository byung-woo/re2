#include <iostream>
#include <string>

#include "re2/re2.h"

int main(int argc, char* argv[]) {
  int64_t max_mem = 1 << 20;

  enum Target {
   kForward = 0b001,
   kReverse = 0b010,
   kMinimumMemLimit = 0b100,
   kAll = kForward | kReverse | kMinimumMemLimit,
  } target = kAll;

  std::vector<std::string> patterns;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-m") == 0) {
      if (++i >= argc) {
        std::cerr << "Need to specify max_mem (byte)." << std::endl;
        std::cerr << " # re2_memprof -m MAX_MEM" << std::endl;
        return 1;
      }
      try {
        max_mem = std::stoi(argv[i]);
      } catch (std::invalid_argument const& e) {
        std::cerr << "Invalid max_mem format: " << e.what() << std::endl;
        return 1;
      }
    } else if (strcmp(argv[i], "-t") == 0) {
      if (++i >= argc) {
        std::cerr << "Need to specify profile target. ( minimum_mem_limit | forward | reverse | all)" << std::endl;
        std::cerr << " # re2_memprof -t PROFILE_TARGET" << std::endl;
        return 1;
      }

      if (strcmp(argv[i], "minimum_mem_limit") == 0) {
        target = kMinimumMemLimit;
      } else if (strcmp(argv[i], "forward") == 0) {
        target = kForward;
      } else if (strcmp(argv[i], "reverse") == 0) {
        target = kReverse;
      } else if (strcmp(argv[i], "all") == 0) {
        target = kAll;
      } else {
        std::cerr << "Invalid profile target: " << argv[i] << std::endl;
        return 1;
      }
    } else if (strcmp(argv[i], "-p") == 0) {
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
    std::cerr << " # re2_memprof -p REGEX_PATTERN" << std::endl;
    std::cerr << " # re2_memprof < patterns.txt" << std::endl;
    return 1;
  }

  re2::RE2::Options opt;
  opt.set_max_mem(max_mem);

  std::cout << "# regex_pattern";
  if (target & kForward) {
    std::cout << "\t" << "forward_nfa_instruction_mem_allocation";
    std::cout << "\t" << "forward_nfa_onepass_mem_allocation";
    std::cout << "\t" << "forward_dfa_mem_budget";
  }
  if (target & kReverse) {
    std::cout << "\t" << "reverse_nfa_instruction_mem_allocation";
    std::cout << "\t" << "reverse_nfa_onepass_mem_allocation";
    std::cout << "\t" << "reverse_dfa_mem_budget";
  }
  if (target & kMinimumMemLimit) {
    std::cout << "\t" << "minimum_mem_limit";
  }
  std::cout << std::endl;

  for (std::string pattern: patterns) {
    re2::RE2 regex(pattern, opt);

    if (!regex.ok()) {
      std::cerr << "Failed to create regex with \"" << pattern << "\"" << std::endl;
      std::cerr << " - error: " << regex.error() << std::endl;
      return 1;
    }

    regex.ForceCreateReverseProg();

    std::cout << pattern;
    if (target & kForward) {
      std::cout << "\t" << regex.ForwardNFAInstructionMemAllocation();
      std::cout << "\t" << regex.ForwardNFAOnepassMemAllocation();
      std::cout << "\t" << regex.ForwardDFAMemBudget();
    }
    if (target & kReverse) {
      std::cout << "\t" << regex.ReverseNFAInstructionMemAllocation();
      std::cout << "\t" << regex.ReverseNFAOnepassMemAllocation();
      std::cout << "\t" << regex.ReverseDFAMemBudget();
    }
    if (target & kMinimumMemLimit) {
      std::cout << "\t" << (regex.MinimumMemBudgetForCreation());
    }
    std::cout << std::endl;

  }
  return 0;
}
