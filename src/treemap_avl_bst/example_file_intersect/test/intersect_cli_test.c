#include <check.h>
#include <file_parser.h>
#include <intersect.h>
#include <stdio.h>
#include <wchar.h>

START_TEST(cli_file_testing) {
  file_tracker_t *tracker = init_tracker();
  const char *files[5] = {"prog", "File2", "File3", "FIle4", "FIle5"};

  parse_args(5, files, tracker);
  ck_assert_int_eq(0, tracker->file_count);

  read_file(tracker);
  free_tracker(tracker);
  ck_assert_msg("Test is for valgrind testing");
}
END_TEST

START_TEST(cli_file_reading) {
  file_tracker_t *tracker = init_tracker();
  const char *files[3] = {"prog", "assets/small_test.txt",
                          "assets/small_test2.txt"};
  parse_args(3, files, tracker);

  read_file(tracker);

  read_file(tracker);
  free_tracker(tracker);

  wprintf(L"\n\n");
  ck_assert_msg("Test is for valgrind testing");
}
END_TEST

START_TEST(cli_file_reading_empty) {

  file_tracker_t *tracker = init_tracker();
  const char *files[3] = {"prog"};
  parse_args(1, files, tracker);

  read_file(tracker);

  read_file(tracker);
  free_tracker(tracker);
  wprintf(L"\n\n");
  ck_assert_msg("Test is for valgrind testing");
}
END_TEST

START_TEST(cli_file_reading_one_file) {

  file_tracker_t *tracker = init_tracker();
  const char *files[3] = {"prog", "assets/small_test.txt"};
  parse_args(2, files, tracker);

  read_file(tracker);

  read_file(tracker);
  free_tracker(tracker);
  ck_assert_msg("Test is for valgrind testing");
}
END_TEST

static TFun creation_default_test[] = {cli_file_testing, cli_file_reading,
                                       cli_file_reading_empty,
                                       cli_file_reading_one_file, NULL};

static void add_tests(TCase *test_cases, TFun *test_functions) {
  while (*test_functions) {
    // add the test from the core_tests array to the t-case
    tcase_add_test(test_cases, *test_functions);
    test_functions++;
  }
}

Suite *intersect_cli_test_suite(void) {
  // suite creation
  Suite *bst_suite = suite_create("Intersection CLI Tester");
  TFun *test_list;
  TCase *test_cases;

  test_list = creation_default_test;
  test_cases = tcase_create("Testing creation and deletion of object");
  add_tests(test_cases, test_list);
  suite_add_tcase(bst_suite, test_cases);

  return bst_suite;
}