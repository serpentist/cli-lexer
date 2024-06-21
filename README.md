# GNU LEXER

This project provides a lexical tokenizer named gnu-lexer.
The lexer is designed to accept specific inputs and,
once executed, generates a sequence of tokens.

The gnu-lexer is built with GNU-style CLI input in mind,
supporting both short (-a) and long (--arg) arguments.

# Building

This project has no external dependencies
but requires support for C++23.

To build the project, run:

```console
cmake -B build
make -C build
```

# Using the API

The resulting library is named gnu-lexer.
To use it, link to the library and include
the directory containing the lexer.hpp header file.

The API provides the lexer\_t class template.
To instantiate the lexer,
specify the type of container in which
you want to store the generated tokens,
such as std::vector.

Once instantiated, create a database of arguments
using the add(argument\_t) member method of the lexer.

The argument\_t object defines the following members:

- token: A unique string returned if a chunk of input
matches the argument's description.

- verbose: The long form of the argument, for example, "help".

- concise: The short form of the argument, for example, "h".

- value.type: The type of value the argument takes (none, single, multi).

- value.delimiter: The delimiter that separates values
for multi arguments (only used for multi type).

After the database is created, call the tokenize() method.

# Examples

For specific examples of how to use the API,
check the demo directory.
