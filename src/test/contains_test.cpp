#include <gnu-lexer/lexer.hpp>
#include <iostream>

int main() {
  std::vector<glex::argument_t> args{};
  constexpr char valdel = ',';
  args.push_back({.token = "id0",
                  .verbose = "arg0",
                  .concise = 'a',
                  .value = {.type = glex::argument_t::value_t::type_t::multi,
                            .delimiter = valdel}});
  args.push_back({.token = "id1",
                  .verbose = "arg1",
                  .concise = 0,
                  .value = {.type = glex::argument_t::value_t::type_t::none,
                            .delimiter = valdel}});
  args.push_back({.token = "id2",
                  .verbose = "arg2",
                  .concise = 'b',
                  .value = {.type = glex::argument_t::value_t::type_t::single,
                            .delimiter = valdel}});
  args.push_back({.token = "id3",
                  .verbose = "arg3",
                  .concise = 'c',
                  .value = {.type = glex::argument_t::value_t::type_t::multi,
                            .delimiter = valdel}});

  glex::argument_t test{
      .token = "id3",
      .verbose = "arg3",
      .concise = 'c',
      .value = {.type = glex::argument_t::value_t::type_t::multi,
                .delimiter = valdel}};

  if (!glex::contains(args, test)) {
    std::cerr << "Failed check 1." << std::endl;
    return 1;
  }

  test.token = "id4";
  if (!glex::contains(args, test)) {
    std::cerr << "Failed check 2." << std::endl;
    return 1;
  }

  test.verbose = "arg4";
  if (!glex::contains(args, test)) {
    std::cerr << "Failed check 3." << std::endl;
    return 1;
  }

  test.concise = 0;
  if (glex::contains(args, test)) {
    std::cerr << "Failed check 4." << std::endl;
    return 1;
  }

  test.concise = 'd';
  if (glex::contains(args, test)) {
    std::cerr << "Failed check 5." << std::endl;
    return 1;
  }

  glex::argument_t a{}, b{};
  a.token = "id0", b.token = "id0";
  a.verbose = "arg0", b.verbose = "arg0";
  a.concise = 'a', b.concise = 'a';

  if (a != b) {
    std::cerr << "Failed check 6." << std::endl;
    return 1;
  }

  b.token = "id1";
  if (a != b) {
    std::cerr << "Failed check 7." << std::endl;
    return 1;
  }

  b.verbose = "arg1";
  if (a != b) {
    std::cerr << "Failed check 8." << std::endl;
    return 1;
  }

  b.concise = 'b';
  if (a == b) {
    std::cerr << "Failed check 9." << std::endl;
    return 1;
  }
}
