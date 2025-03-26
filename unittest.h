#ifndef UNITTEST_H_
#define UNITTEST_H_

#define ASSERT(condition, name) unittest_assert(condition, name, __FILE__, __LINE__)
#define ASSERT_STR(a, b, name) unittest_assert_str(a, b, name, __FILE__, __LINE__)

int unittest_assert(int condition, char *name, char *file, int line);
int unittest_assert_str(char *a, char *b, char *name, char *file, int line);
int test_main();

#endif