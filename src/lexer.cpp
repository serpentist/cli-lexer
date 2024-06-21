#include <cctype>
#include <gnu-lexer/lexer.hpp>

namespace glex {
bool is_valid(const argument_t &arg) {
  if (!arg.token.size())
    return false;
  if (!arg.verbose.size())
    return false;
  if (arg.value.type == argument_t::value_t::type_t::multi) {
    switch (arg.value.delimiter) {
    case 0:
    case '-':
    case ' ':
      return false;
    }
    if (std::isalnum(arg.value.delimiter))
      return false;
  }
  return true;
}

bool contains(const std::list<argument_t> &db, const argument_t &b) {
  for (const auto &a : db)
    if (a == b)
      return true;
  return false;
}

bool contains(const std::vector<argument_t> &db, const argument_t &b) {
  for (const auto &a : db)
    if (a == b)
      return true;
  return false;
}
} // namespace glex

bool operator==(const glex::argument_t &a, const glex::argument_t &b) {
  return !(a != b);
}
bool operator!=(const glex::argument_t &a, const glex::argument_t &b) {
  return (a.token != b.token) && (a.verbose != b.verbose) &&
         (a.concise && b.concise ? a.concise != b.concise : true);
}

namespace glex {
int count(int argc, char **argv, const std::string &pat, int skip) {
  int score{};
  for (int i = skip; i < argc; ++i)
    if (argv[i] == pat)
      ++score;
  return score;
}

std::vector<std::string> read(char **argv, int count, int skip) {
  std::vector<std::string> out{};
  out.reserve(count);
  for (int i = 0; i < count; ++i)
    out.push_back(argv[i + skip]);
  return out;
}

std::vector<std::string> read(int argc, char **argv, const std::string &end,
                              int &skip) {
  std::vector<std::string> out{};
  for (; skip < argc; ++skip) {
    if (argv[skip] == end)
      break;
    out.push_back(argv[skip]);
  }
  return out;
}
} // namespace glex
