#include <tmb_test.h>

static test_suite_t tests      = { 0 };
test_case_t* current_test      = NULL;
static test_stats_t test_stats = { 0 };

int indent = 0;

void register_test(const char* suite_name, const char* name, test_fn func) {
    da_append(&tests,
              ((test_case_t) {
                      .func = func, .name = name, .suite_name = suite_name }));
}

#define MAX_INDENT 24
static char spaces[MAX_INDENT] = "                       ";

static void indent_push() {
    indent += 4;
    if (indent > 24) indent = 24;
}

static void indent_pop() {
    indent -= 4;
    if (indent < 0) indent = 0;
}

static void indent_reset() {
    indent = 0;
}

#define PRINTF(...)                                                            \
    do {                                                                       \
        printf("%.*s", indent, spaces);                                        \
        printf(__VA_ARGS__);                                                   \
    } while (0)

static void print_fail(assertion_fail_meta_t* meta) {
    PRINTF("ASSERTION FAILED :  %s   %s:%d \n",
           meta->message.items,
           meta->file,
           meta->line);
}

static int RUN_ALL_TESTS() {
    const char* current_suite = NULL;
    test_stats_t suite_stats  = { 0 };

    for (int i = 0; i < tests.length; i++) {
        current_test = &tests.items[i];

        if (current_suite == NULL ||
            strcmp(current_suite, current_test->suite_name) != 0) {
            if (current_suite != NULL) {
                PRINTF("\n");
            }

            indent_reset();
            current_suite = current_test->suite_name;
            suite_stats   = (test_stats_t) { 0 };
            // PRINTF("Test Suite: %s\n", current_suite);
        }

        current_test->func();

        current_test->stats.passed = current_test->stats.total -
                                     current_test->stats.failed;

        suite_stats.failed += current_test->stats.failed;
        suite_stats.total += current_test->stats.total;
        suite_stats.passed += current_test->stats.passed;

        test_stats.failed += current_test->stats.failed;
        test_stats.total += current_test->stats.total;
        test_stats.passed += current_test->stats.passed;

        if (current_test->stats.failed > 0) {
            PRINTF("%s\n", current_test->name);
            indent_push();
            PRINTF("total: %d  passed: %d  failed: %d\n",
                   current_test->stats.total,
                   current_test->stats.passed,
                   current_test->stats.failed);

            indent_push();
            for (int j = 0; j < current_test->failed.length; j++) {
                print_fail(&current_test->failed.items[j]);
            }
            indent_pop();
            indent_pop();
        } else {
            // PRINTF("✓ %s\n", current_test->name);
        }
    }

    if (current_suite != NULL) {
        indent_pop();
        PRINTF("\n");
    }

    indent_reset();
    PRINTF("OVERALL SUMMARY\n");
    indent_push();
    PRINTF("total     passed    failed\n");
    PRINTF("%d         %d         %d\n",
           test_stats.total,
           test_stats.passed,
           test_stats.failed);
    PRINTF("\n");
    return test_stats.failed;
}

int main(int argc, char* argv[]) {
    UNUSED argc;
    UNUSED argv;
    return RUN_ALL_TESTS();
}
