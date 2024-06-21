#include <gnu-lexer/lexer.hpp>
#include <iostream>

// This test takes the following input:
// <what> <count> <list>
// and returns true if <what> occurs <count> times in <list>,
// and false otherwise.

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr << "ERR: Invalid input; Usage: <what> <count> <list>\n";
    return 1;
  }
  std::string what{argv[1]};
  std::string cntstr{argv[2]};

  try {
    auto expected_count = std::stoi(cntstr);
    auto count = glex::count(argc, argv, what, 3);
    if (expected_count != count) {
      std::cerr << "The expected count (" << expected_count << ") ";
      std::cerr << "does not match the actual count (" << count << ")\n";
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "ERR: " << e.what() << std::endl;
  }
}
