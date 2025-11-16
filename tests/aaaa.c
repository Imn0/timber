#include <tmb_test.h>

static int add(int a, int b) {
    return a + b;
}

static int multiply(int a, int b) {
    return a * b;
}

static int divide(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}

TEST(MathOperations, addition_positive_numbers) {
    ASSERT_EQ(add(2, 3), 5);
    ASSERT_EQ(add(10, 20), 30);
    ASSERT_EQ(add(100, 200), 300);
}

TEST(MathOperations, addition_negative_numbers) {
    ASSERT_EQ(add(-5, -3), -8);
    ASSERT_EQ(add(-10, 5), -5);
}

TEST(MathOperations, multiplication_works) {
    ASSERT_EQ(multiply(3, 4), 12);
    ASSERT_EQ(multiply(0, 100), 0);
    ASSERT_EQ(multiply(-2, 5), -10);
}

TEST(MathOperations, division_works) {
    ASSERT_EQ(divide(10, 2), 5);
    ASSERT_EQ(divide(100, 10), 10);
}

TEST(StringOperations, string_comparison) {
    const char* str1 = "hello";
    const char* str2 = "hello";
    ASSERT_STR_EQ(str1, str2);
}

TEST(StringOperations, empty_strings) {
    const char* str1 = "";
    const char* str2 = "";
    ASSERT_STR_EQ(str1, str2);
}

TEST(BooleanLogic, boolean_assertions) {
    int x = 10;
    ASSERT2(x < 5);
    ASSERT2(x < 20);
    ASSERT2(x == 10);
}

TEST(BooleanLogic, comparison_operators) {
    ASSERT2(5 > 3);
    ASSERT2(10 >= 10);
    ASSERT2(2 != 3);
}

TEST(EdgeCases, zero_values) {
    ASSERT_EQ(add(0, 0), 0);
    ASSERT_EQ(multiply(0, 0), 0);
}

TEST(EdgeCases, large_numbers) {
    ASSERT_EQ(add(1000000, 2000000), 3000000);
}
