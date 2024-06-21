#include <gnu-lexer/lexer.hpp>
#include <iostream>

namespace {
std::size_t err(const std::size_t c, const std::string &msg = "Failed check ") {
  std::cerr << msg << c << std::endl;
  return c;
}
} // namespace

int main() {
  std::size_t check{0};
  glex::argument_t test{};

  if (++check; glex::is_valid(test))
    return err(check);

  test.token = "arg0";
  if (++check; glex::is_valid(test))
    return err(check);

  test.verbose = "arg0";
  if (++check; !glex::is_valid(test))
    return err(check);

  test.token = "id0";
  if (++check; !glex::is_valid(test))
    return err(check);

  test.value.type = glex::argument_t::value_t::type_t::multi;
  if (++check; glex::is_valid(test))
    return err(check);

  test.value.delimiter = 'a';
  if (++check; glex::is_valid(test))
    return err(check);

  test.value.delimiter = ' ';
  if (++check; glex::is_valid(test))
    return err(check);

  test.value.delimiter = '-';
  if (++check; glex::is_valid(test))
    return err(check);

  test.value.delimiter = 0;
  if (++check; glex::is_valid(test))
    return err(check);

  test.value.delimiter = ':';
  if (++check; !glex::is_valid(test))
    return err(check);

  test.value.delimiter = ',';
  if (++check; !glex::is_valid(test))
    return err(check);
}
