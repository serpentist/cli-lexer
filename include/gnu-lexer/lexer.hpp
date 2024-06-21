#pragma once
#include <iostream>
#include <list>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace glex {
struct argument_t {
  std::string token;

  std::string verbose; // long argument version e.g. --opt
  char concise;        // short argument version e.g. -o

  struct value_t {
    enum class type_t { none, single, multi };
    type_t type;
    char delimiter{0};
  };
  value_t value;
};

bool contains(const std::list<argument_t> &, const argument_t &);
bool contains(const std::vector<argument_t> &, const argument_t &);
bool is_valid(const argument_t &);

struct token_t {
  std::string id;
  std::vector<std::string> values;
};
} // namespace glex

bool operator!=(const glex::argument_t &a, const glex::argument_t &b);
bool operator==(const glex::argument_t &a, const glex::argument_t &b);

namespace glex {
/* Reads elements from container b, that has a elements,
 * starting at element with index d,
 * up to element with value c, excluding c.
 *
 * Index d is updated to the index
 * at which the element c was found.
 */
std::vector<std::string> read(int a, char **b, const std::string &c, int &d);

/* Reads b elements from container a,
 * starting at element with index c.
 * No error checking is performed.
 */
std::vector<std::string> read(char **a, int b, int c = 1);

/* Counts the number of elements of container b that
 * are equal to c, using d as the starting offset.
 */
int count(int a, char **b, const std::string &c, int d = 1);
} // namespace glex

namespace glex {
template <template <typename, typename...> typename ContainerType>
class lexer_t {
public:
  using container_t = ContainerType<token_t>;

  container_t tokenize(int argc, char **argv, int skip = 1) const {
    return tokenize(read(argv, argc, 0), skip);
  }

  using input_t = std::vector<std::string>;
  using offset_t = typename input_t::size_type;
  container_t tokenize(const input_t &, const offset_t & = 0) const;

  void add(argument_t arg) {
    if (!is_valid(arg) || contains(argdb_, arg)) {
      throw std::runtime_error{"The supplied arg is invalid!"};
    }
    argdb_.push_back(std::move(arg));
    verbosedb_.emplace(argdb_.back().verbose, &argdb_.back());
    if (argdb_.back().concise)
      concisedb_.emplace(argdb_.back().concise, &argdb_.back());
    tokendb_.emplace(argdb_.back().token, &argdb_.back());
  }

  void clear() {
    argdb_.clear();
    verbosedb_.clear();
    concisedb_.clear();
    tokendb_.clear();
  };

  void debug(bool v) { dbg_ = v; }
  bool debug() const { return dbg_; }

private:
  void assign(const std::string &) const;
  void tokenize(const std::string &) const;
  bool handle_value(const std::string &chunk) const;
  bool handle_arglist(const std::string &chunk) const;
  bool is_arglist(const std::string &chunk) const;
  bool handle_longarg(const std::string &chunk) const;
  bool is_longarg(const std::string &chunk) const;
  bool handle_freearg(const std::string &chunk) const;

  void logdbg(const std::string &s) const {
    if (debug())
      std::cout << "DBG: " << s << std::endl;
  }

private:
  std::list<argument_t> argdb_;
  std::unordered_map<std::string, argument_t *> verbosedb_;
  std::unordered_map<std::string, argument_t *> tokendb_;
  std::unordered_map<char, argument_t *> concisedb_;
  mutable container_t tokens_;
  mutable bool hyphen_{false};
  mutable bool value_{false};
  mutable bool skip_{false};
  bool dbg_{false};
};
} // namespace glex

/************************************* IMPLEMENTATION *************************/
namespace glex {
template <template <typename, typename...> typename C>
void lexer_t<C>::assign(const std::string &v) const {
  std::string val{v.front() == '=' ? v.begin() + 1 : v.begin(), v.end()};
  if (!val.size())
    throw std::runtime_error{"An assigned value cannot be empty."};

  auto active = tokendb_.at(tokens_.back().id);
  using avt = argument_t::value_t::type_t;
  if (active->value.type == avt::single) {
    tokens_.back().values.push_back(std::move(val));
    return;
  }

  using namespace std::ranges;
  tokens_.back().values = views::split(val, active->value.delimiter) |
                          views::transform([](const auto &s) {
                            return std::string{s.begin(), s.end()};
                          }) |
                          to<std::vector>();
  if (tokens_.back().values.back().empty())
    tokens_.back().values.pop_back();
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::handle_value(const std::string &chunk) const {
  if (!value_)
    return false;
  value_ = false;

  logdbg("Chunk identified as: value");

  assign(chunk);
  return true;
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::is_arglist(const std::string &chunk) const {
  if (hyphen_)
    return false;
  if (chunk.size() < 2)
    throw std::runtime_error{
        "Hyphen mode is not active, and the current chunk is not supposed to "
        "be a value, so the chunk must be at least 2 characters long."};
  if (chunk[0] != '-')
    return false;
  if (chunk[1] == '-')
    return false;
  return true;
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::handle_arglist(const std::string &chunk) const {
  if (!is_arglist(chunk))
    return false;
  logdbg("Chunk identified as: arglist");

  typename std::string::size_type finarg = 1;
  using avt = argument_t::value_t::type_t;

  for (; finarg < chunk.size(); ++finarg) {
    if (!std::isalpha(chunk[finarg]))
      throw std::runtime_error{"An argument list must only contain letters "
                               "apart from the starting dash."};
    if (!concisedb_.contains(chunk[finarg]))
      throw std::runtime_error{"The character: '" + std::string{chunk[finarg]} +
                               "' is not a valid concise argument."};
    auto arg = concisedb_.at(chunk[finarg]);
    if (tokens_.back().id.size() || tokens_.back().values.size())
      tokens_.push_back({.id = arg->token, .values = {}});
    else
      tokens_.back() = {.id = arg->token, .values = {}};

    if (arg->value.type != avt::none)
      break;
  }

  if (++finarg >= chunk.size()) {
    if (tokendb_.at(tokens_.back().id)->value.type != avt::none)
      value_ = true;
    return true;
  }

  assign(chunk.substr(finarg, chunk.size() - finarg));
  return true;
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::is_longarg(const std::string &chunk) const {
  if (hyphen_)
    return false;
  if (chunk.size() < 3)
    return false;
  if (chunk[0] != '-')
    return false;
  if (chunk[1] != '-')
    return false;
  if (!std::isalpha(chunk[2]))
    throw std::runtime_error{
        "The first character of a verbose argument must be a letter."};
  return true;
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::handle_longarg(const std::string &chunk) const {
  if (!is_longarg(chunk))
    return false;
  logdbg("Chunk identified as: longarg");

  std::string vname{chunk.begin() + 2, chunk.end()};
  typename std::string::size_type eqpos{};
  eqpos = chunk.find('=');
  std::string value{};

  if (eqpos != std::string::npos) {
    value = chunk.substr(eqpos + 1, chunk.size() - (eqpos + 1));
    vname = chunk.substr(2, eqpos - 2);
  }

  if (!verbosedb_.contains(vname))
    throw std::runtime_error{"The specified long arg: '" + vname +
                             "' is not in the database."};

  auto desc = verbosedb_.at(vname);
  tokens_.back().id = desc->token;

  using avt = argument_t::value_t::type_t;
  if (desc->value.type == avt::none) {
    if (value.size())
      throw std::runtime_error{"The flag: '" + vname +
                               "' does not take any parameters."};
    return true;
  }
  if (value.size())
    assign(value);
  else
    value_ = true;
  return true;
}

template <template <typename, typename...> typename C>
bool lexer_t<C>::handle_freearg(const std::string &chunk) const {
  logdbg("Chunk identified as: freearg");
  if (!hyphen_ && chunk == "--") {
    hyphen_ = true;
    skip_ = true;
    return true;
  }
  tokens_.back().values.push_back(chunk);
  return true;
}

template <template <typename, typename...> typename C>
void lexer_t<C>::tokenize(const std::string &chunk) const {
  logdbg("Analyzing chunk: '" + chunk + "'");
  try {
    if (handle_value(chunk)) // ... -o value ...
      return;
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Failed handle_value(...): "} +
                             e.what()};
  }
  if (!skip_)
    tokens_.push_back({});
  else
    skip_ = false;

  try {
    if (handle_arglist(chunk)) // ... -abcd ...
      return;
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Failed handle_arglist(...): "} +
                             e.what()};
  }

  try {
    if (handle_longarg(chunk)) // ... --arg ...
      return;
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Failed handle_longarg(...): "} +
                             e.what()};
  }

  try {
    if (handle_freearg(chunk)) // ... freeval ...
      return;
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Failed handle_freearg(...): "} +
                             e.what()};
  }
}

template <template <typename, typename...> typename ContainerType>
lexer_t<ContainerType>::container_t
lexer_t<ContainerType>::tokenize(const input_t &in, const offset_t &off) const {
  if (!in.size())
    return {};
  if (tokens_.size())
    tokens_.clear();

  hyphen_ = false, value_ = false, skip_ = false;

  for (typename input_t::size_type i = off; i < in.size(); ++i)
    tokenize(in[i]);

  using avt = argument_t::value_t::type_t;
  for (const auto &t : tokens_)
    if (t.id.size() && tokendb_.at(t.id)->value.type != avt::none)
      if (t.values.empty())
        throw std::runtime_error{"The token: '" + t.id + "' requires a value."};
  return tokens_;
}
} // namespace glex
