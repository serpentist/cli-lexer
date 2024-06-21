add_executable(test-counter test_counter.cpp)
target_link_libraries(test-counter PRIVATE gnu-lexer)
add_test(NAME counter_test_pass_01 COMMAND test-counter
  ";" 5 ";" hello ";" ";" ";" test ";" something else)

add_test(NAME counter_test_pass_02 COMMAND test-counter
  "sth" 0 hello ";" ";" ";" test ";" something else)

add_test(NAME counter_test_fail_01 COMMAND test-counter
  "sth" 0 hello ";" "sth" ";" test ";" something else)

set_tests_properties(counter_test_fail_01 PROPERTIES WILL_FAIL true)

add_executable(read-test read_test.cpp)
target_link_libraries(read-test PRIVATE gnu-lexer)
add_test(NAME read_function_test COMMAND read-test)

add_executable(contains-test contains_test.cpp)
target_link_libraries(contains-test PRIVATE gnu-lexer)
add_test(NAME contains_function_test COMMAND contains-test)

add_executable(is-valid-test is_valid_test.cpp)
target_link_libraries(is-valid-test PRIVATE gnu-lexer)
add_test(NAME is_valid_function_test COMMAND is-valid-test)

add_executable(test-lexer test_lexer.cpp)
target_link_libraries(test-lexer PRIVATE gnu-lexer)
add_test(NAME lexer_test_pass_01 COMMAND test-lexer
  help:help:h:none:0 ";" -h ";" help
)
add_test(NAME lexer_test_pass_02 COMMAND test-lexer
  help:help:h:none:0 ";" -hhhhh ";" help help help help help
)
add_test(NAME lexer_test_pass_03 COMMAND test-lexer
  help:help:h:none:0 analyze:analyze:a:multi:, ";" -h ";" help
)
add_test(NAME lexer_test_pass_04 COMMAND test-lexer
  help:help:h:none:0 analyze:analyze:a:multi:, ";"
  -havalue,valu2 ";" help analyze:value:valu2
)
add_test(NAME lexer_test_pass_05 COMMAND test-lexer
  help:help:h:none:0 analyze:analyze:a:multi:, ";"
  --help -h --analyze=value,valu2 -asth -a sth ";"
  help help analyze:value:valu2 analyze:sth analyze:sth
)
add_test(NAME lexer_test_pass_06 COMMAND test-lexer
  analyze:analyze:a:multi:,
  extract:extract:e:none:,
  ";"
  --analyze -freeval --analyze=value
  ";"
  analyze:-freeval analyze:value
)
add_test(NAME lexer_test_pass_07 COMMAND test-lexer
  flag:flag:f:none:0
  ";"
  --flag -- -weird-file-name-
  ";"
  flag :-weird-file-name-
)
add_test(NAME lexer_test_pass_08 COMMAND test-lexer
  flag:flag:f:none:0
  ";"
  --flag -- --
  ";"
  flag :--
)
add_test(NAME lexer_test_pass_09 COMMAND test-lexer
  help:help:h:none:0
  extr:extract:e:none:0
  anlz:analyze:a:none:0
  prof:profile:p:single:0
  file:files:f:multi:,
  ";"
  --help -eap/path/to/prof -f=f1,f2,f3 -- --val
  --profile /path/to/profile -ef /some/file
  ";"
  help extr anlz prof:/path/to/prof file:f1:f2:f3
  :--val :--profile :/path/to/profile :-ef :/some/file
)
add_test(NAME lexer_test_pass_10 COMMAND test-lexer
  help:help:h:none:0
  extr:extract:e:none:0
  anlz:analyze:a:none:0
  prof:profile:p:single:0
  file:files:f:multi:,
  ";"
  --help -eap/path/to/prof -f=f1,f2,f3 val
  --profile /path/to/profile -ef /some/file
  ";"
  help extr anlz prof:/path/to/prof file:f1:f2:f3
  :val prof:/path/to/profile extr file:/some/file
)
add_test(NAME lexer_test_fail_01 COMMAND test-lexer
  analyze:analyze:a:multi:, ";" --analyze ";" analyze
)
add_test(NAME lexer_test_fail_02 COMMAND test-lexer
  help:help:h:none:0
  extr:extract:e:none:0
  anlz:analyze:a:none:0
  prof:profile:p:single:0
  file:files:f:multi:,
  ";"
  -heazf free sth
  ";"
  desn't matter will fail
)
set_tests_properties(
  lexer_test_fail_01
  lexer_test_fail_02
  PROPERTIES WILL_FAIL true
)
