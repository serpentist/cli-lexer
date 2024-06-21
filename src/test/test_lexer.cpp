#include <gnu-lexer/lexer.hpp>

/* This test takes the following input:
 * <def>... ; <arg>... ; <out>...
 *
 * where:
 * <def> is the list of argument definitions in the form:
 *  <id>:<long>:<short>:<none|single|multi>:<vdelim>
 *
 * <inp> is the test input such as:
 *  -o val --flag
 *
 * <out> is the list of expected ids and their values in the form:
 *  <id>:<val>[:<val>]...
 *
 * After parsing the input, if it's valid,
 * the lexer generates tokens from the list of <arg>...
 * and afterwards that output is compared to
 * the expected output.
 */

template <typename T> void print(const T &tokens, const std::string &msg) {
  auto tok = tokens.begin();
  std::cout << msg;
  for (std::size_t i = 0; i < tokens.size(); ++i) {
    std::cout << "'" << tok->id;
    if (tok->values.size()) {
      for (const auto &v : tok->values)
        std::cout << ":" << v;
    }
    std::cout << "'";
    tok = std::next(tok);
    if (tok != tokens.end())
      std::cout << " ";
  }
  std::cout << std::endl;
}

template <typename T, typename U>
bool verify(const T &tokens, const U &expected) {
  if (tokens.size() != expected.size()) {
    std::cerr << "ERR: The amount of generated tokens (" << tokens.size();
    std::cerr << ") is not as expected (" << expected.size()
              << "):" << std::endl;
    print(tokens, "TOKENS: ");
    print(expected, "EXPECTED: ");
    return false;
  }

  auto tok = tokens.begin();
  for (std::size_t i = 0; i < tokens.size(); ++i) {
    if (tok->id != expected[i].id) {
      std::cerr << "ID mismatch; Expected: '" << expected[i].id;
      std::cerr << "', got: '" << tok->id << "'" << std::endl;
      return false;
    }

    if (expected[i].values.size() != tok->values.size()) {
      std::cerr << "The amount of generated token values ("
                << tok->values.size();
      std::cerr << ") is not as expected (" << expected[i].values.size()
                << ").\n";
      return false;
    }

    for (auto exp = expected[i].values.begin(), it = tok->values.begin();
         it != tok->values.end(); ++exp, ++it) {
      if (*exp != *it) {
        std::cerr << "Value mismatch; Expected: '" << *exp << "',";
        std::cerr << " got: '" << *it << "'" << std::endl;
        return false;
      }
    }
    tok = std::next(tok);
  }
  return true;
}

std::vector<std::string> split_def(const std::string &def,
                                   const std::string &delim2) {
  using namespace std::ranges;
  auto split = views::split(def, delim2) | views::transform([](const auto &s) {
                 return std::string{s.begin(), s.end()};
               }) |
               to<std::vector>();

  if (split.size() != 5)
    throw std::runtime_error{"A definition must have exactly five fields."};
  if (split[2].size() != 1)
    throw std::runtime_error{"A short argument name must be 1 char long."};

  std::string valt = split[3];
  if (valt != "none" && valt != "single" && valt != "multi")
    throw std::runtime_error{
        "The value type must be one of none, single, or multi."};
  if (valt == "multi" && split[4].size() != 1)
    throw std::runtime_error{
        "The value delimiter must be 1 character for multi value options."};
  return split;
}

template <typename T>
void append(const std::string &def, const std::string &delim2, T &lexer) {
  std::vector<std::string> split{};
  try {
    split = split_def(def, delim2);
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Definition splitting failed: "} +
                             e.what()};
  }

  using vt = glex::argument_t::value_t::type_t;
  vt vtype{};
  if (split[3] == "none")
    vtype = vt::none;
  else if (split[3] == "single")
    vtype = vt::single;
  else
    vtype = vt::multi;

  std::cout << "adding({.token = " << split[0] << ", .verbose = " << split[1]
            << ", ";
  std::cout << ".concise = " << split[2][0] << ", .vt = " << split[3] << ", ";
  std::cout << ".delimiter = " << split[4][0] << "})\n";

  try {
    lexer.add({.token = split[0],
               .verbose = split[1],
               .concise = (split[2][0] == '0') ? char{0} : split[2][0],
               .value = {.type = vtype,
                         .delimiter = split[4][0] ? split[4][0] : char{0}}});
  } catch (const std::exception &e) {
    throw std::runtime_error{std::string{"Appending database failed: "} +
                             e.what()};
  }
}

bool verify(const std::string &delim, int argc, char **argv) {
  if (argc < 6) {
    std::cerr
        << "ERR: Invalid input; Expected: <def>... ; <arg>... ; <out>...\n";
    return false;
  }

  if (auto c = glex::count(argc, argv, delim); c != 2) {
    std::cerr << "ERR: The '" << delim << "' character count must be 2.\n";
    std::cerr << "Counted " << c << "occurrence(s)." << std::endl;
    return false;
  }

  // Make sure two delimiters don't occur right after each other.
  char *prev = argv[0];
  for (int i = 1; i < argc; ++i) {
    if (std::string{prev} == std::string{argv[i]}) {
      if (std::string{prev} == delim) {
        std::cerr << "The main delimiter can't occur twice in a row."
                  << std::endl;
        return false;
      }
    }
    prev = argv[i];
  }
  return true;
}

template <typename T>
std::vector<glex::token_t> process(const T &outs, const std::string &delim2) {
  std::vector<glex::token_t> expected{};
  using namespace std::ranges;
  for (const auto &e : outs) {
    auto split = views::split(e, delim2) | views::transform([](const auto &e) {
                   return std::string{e.begin(), e.end()};
                 }) |
                 to<std::vector>();
    expected.push_back(
        {.id = split.front(), .values = {split.begin() + 1, split.end()}});
  }
  return expected;
}

int main(int argc, char **argv) {
  const std::string delim{";"};
  const std::string delim2{":"};
  int begin = 1;

  if (!verify(delim, argc, argv))
    return 1;

  std::vector<std::string> defs{}, inpt{};
  std::vector<glex::token_t> expected{};
  try {
    defs = glex::read(argc, argv, delim, begin);
    inpt = glex::read(argc, argv, delim, ++begin);
    expected = process(glex::read(argv, argc - (begin + 1), begin + 1), delim2);
  } catch (const std::exception &e) {
    std::cerr << "ERR: parsing input failed: " << e.what() << std::endl;
    return 1;
  }

  using llexer_t = glex::lexer_t<std::list>;
  std::vector<glex::argument_t> args{};
  llexer_t lexer{};
  lexer.debug(true);

  try {
    for (auto &&def : defs)
      append(def, delim2, lexer);
  } catch (const std::exception &e) {
    std::cerr << "ERR: creating database failed: " << e.what() << std::endl;
    return 1;
  }

  typename llexer_t::container_t tokens{};
  try {
    tokens = lexer.tokenize(inpt);
  } catch (const std::exception &e) {
    std::cerr << "ERR: tokenization failed: " << e.what() << std::endl;
    return 1;
  }

  if (!verify(tokens, expected))
    return 1;
  print(tokens, "TOKENS: ");
}
