#include <gnu-lexer/lexer.hpp>

/* In this example we create a lexer for a
 * hypothetical utility called nodectrl,
 * that can start a list of nodes.
 *
 * To supplement the lexer, we will write a small parser
 * that will analyze the generated tokens
 * and provide a meaningful error message,
 * if the input is incorrect.
 *
 * The following input should be accepted,
 * and are all equivalent:
 *
 * ./demo/nodectrl -s --list 1,2
 * ./demo/nodectrl -s --list=1,2
 * ./demo/nodectrl -sl1,2
 * ./demo/nodectrl --start -l1,2
 * ./demo/nodectrl -sl 1,2
 * ./demo/nodectrl -sl=1,2
 * ./demo/nodectrl --start --list=1,2
 */

void parse(const std::list<glex::token_t>&);

int main(int argc, char** argv) {
  using av_t = glex::argument_t::value_t::type_t;
  using tokenizer_t = glex::lexer_t<std::list>;
  try {
    tokenizer_t lex{};
    // lex.debug(true); // Enable this for debug prints

    /* First, we are going to create the argument database. */
    lex.add({.token = "nodes",
        .verbose = "list",
        .concise = 'l',
        .value = {.type = av_t::multi, .delimiter = ','}
    });
    lex.add({.token = "start",
        .verbose = "start",
        .concise = 's',
        .value = {.type = av_t::none, .delimiter = 0}
    });

    /* Next, we are going to process the supplied input. */
    parse(lex.tokenize(argc, argv));
  }
  catch (const std::exception& e) {
    std::cerr << "ERR: " << e.what() << std::endl;
    return 1;
  }
}

template <typename T>
void parse_start(T& t) {
  if (!t.size() || t.front().id != "nodes")
    throw std::runtime_error{"The start action requires a list of nodes."};
  std::cout << "starting nodes ";
  for (std::size_t i = 0; i < t.front().values.size(); ++i) {
    std::cout << t.front().values[i];
    if (i != t.front().values.size() -1)
      std::cout << ", ";
  }
  std::cout << std::endl;
  t.pop_front();
}

void parse(const std::list<glex::token_t>& toks) {
  static std::unordered_map<std::string, std::vector<std::string>> actions = {
    {"start", {"nodes"}}
  };

  std::list<glex::token_t> tokens{toks.begin(), toks.end()};
  if (!tokens.size())
    throw std::runtime_error{"No tokens"};

  try {
    while (tokens.size()) {
      if (tokens.front().id == "start") {
        tokens.pop_front();
        parse_start(tokens);
      }
      if (tokens.front().id.empty())
        throw std::runtime_error{"Free values are not allowed."};
    }
  } catch (const std::exception& e) {
    throw std::runtime_error{std::string{"Parsing failed: "} + e.what()};
  }
}
