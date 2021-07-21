#include <stdio.h>
#include <stdlib.h>
#include <bst.h>
#include <check.h>
#include <math.h>

START_TEST(test_poly_term_create)
{
    ck_assert_int_eq(10, 10);

} END_TEST

Suite * test_poly_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("BST Test");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_poly_term_create);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_poly_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
