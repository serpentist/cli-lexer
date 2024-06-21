#include <gnu-lexer/lexer.hpp>
#include <iostream>

// This test takes no input, and checks if the glex::read function
// works as expected.

int main() {
  constexpr std::size_t inpsize = 15;
  constexpr std::size_t strsize = 50;
  char input[inpsize][strsize] = {{"arg0:flg0:a:none"},
                                  {"arg1:flg1:0:none"},
                                  {"arg2:opt2:b:single"},
                                  {"arg3:opt3:c:multi:,"},
                                  {";"},
                                  {"-a"},
                                  {"--flg1"},
                                  {"--opt2 value"},
                                  {"-ac value1,value2,value3"},
                                  {";"},
                                  {"arg0"},
                                  {"arg1"},
                                  {"arg2"},
                                  {"arg0"},
                                  {"arg3"}};

  char *inpptr[inpsize];
  for (std::size_t i = 0; i < inpsize; ++i)
    inpptr[i] = input[i];

  /************** READ UNTIL TEST ******************/
  {
    int begin = 0, skip = 0;

    // reads until first ';'
    auto chunks = glex::read(inpsize, inpptr, ";", skip);

    auto check = [&](int begin, int end) {
      if (static_cast<int>(chunks.size()) != end - begin) {
        std::cerr << "The size of chunks (" << chunks.size() << ") ";
        std::cerr << "is not as expected (" << end - begin << ")\n";
        return false;
      }
      for (int i = begin, j = 0; i < end; ++i, ++j) {
        std::cout << "(chunks[" << j << "]: '" << chunks[j] << "' == ";
        std::cout << "input[" << i << "]: '" << input[i] << "') <=> ";
        if (chunks[j] != input[i]) {
          std::cout << "false\n";
          return false;
        }
        std::cout << "true\n";
      }
      return true;
    };

    if (!check(begin, skip))
      return 1;
    std::cout << std::endl;
    begin = ++skip;

    // reads until next ';'
    chunks = glex::read(inpsize, inpptr, ";", skip);

    if (!check(begin, skip))
      return 2;
    std::cout << std::endl;
  }
  std::cout << "READ UNTIL TEST PASSED\n" << std::endl;

  /************** READ COUNT TEST ******************/
  {
    int offset = 10, count = 5;
    auto chunks = glex::read(inpptr, count, offset);
    std::vector<std::string> expected = {
        {"arg0"}, {"arg1"}, {"arg2"}, {"arg0"}, {"arg3"}};

    if (chunks.size() != expected.size()) {
      std::cerr << "The sizes of chunks and expected do not match!\n";
      return 3;
    }
    for (int i = 0; i < static_cast<int>(expected.size()); ++i) {
      std::cout << "(chunks[" << i << "]: '" << chunks[i] << "' == ";
      std::cout << "expected[" << i << "]: '" << expected[i] << "') <=> ";
      if (expected[i] != chunks[i]) {
        std::cout << "false\n";
        return 3;
      }
      std::cout << "true\n";
    }
  }
  std::cout << "\nREAD COUNT TEST PASSED" << std::endl;
}
