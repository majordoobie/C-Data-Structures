#include <bst.h>
#include <check.h>
#include <stdlib.h>
#include <tree_map_avl.h>

typedef struct bst_node_payload_t {
  char *student_name;
  double grade_avg;
} key_val_t;

typedef struct {
  double grade;
  key_val_t *payload;
} test_value_search_t;

bst_compare_t map_compare(key_val_t *current_payload, key_val_t *new_payload) {
  int val = strcmp(current_payload->student_name, new_payload->student_name);
  if (0 == val) {
    return BST_EQ;
  }
  // If it's larger, then the current comes first
  else if (val > 0) {
    return BST_LT;
  } else {
    return BST_GT;
  }
}

void map_free_payload(key_val_t *node_payload) {
  free(node_payload->student_name);
  free(node_payload);
}

key_val_t *create_key_val_t(const char *name, double grade_avg) {
  key_val_t *student = calloc(1, sizeof(*student));

  // Copy name into struct
  size_t str_size = strlen(name) + 1;
  student->student_name = malloc(sizeof(char) * str_size);
  strncpy(student->student_name, name, str_size);
  student->grade_avg = grade_avg;
  return student;
}

bst_recurse_t map_stop_recurse(key_val_t *node_payload, void *ptr) {
  test_value_search_t *match = (test_value_search_t *)ptr;
  if ((int)node_payload->grade_avg == (int)match->grade) {
    match->payload = node_payload;
    return RECURSE_STOP;
  }
  return RECURSE_TRUE;
}

dict_t *create_dict() {
  return init_dict(map_compare, map_free_payload, map_stop_recurse);
}

dict_t *create_multi_dict() {
  dict_t *dict = init_dict(map_compare, map_free_payload, map_stop_recurse);

  put_key_val(dict, create_key_val_t("Johnson Caputo", 74));
  put_key_val(dict, create_key_val_t("johnson Caputo", 106));
  put_key_val(dict, create_key_val_t("Steven Solis", 83));
  put_key_val(dict, create_key_val_t("Roger Williams", 82));
  put_key_val(dict, create_key_val_t("Roger Carl", 81));
  put_key_val(dict, create_key_val_t("K", 75));
  put_key_val(dict, create_key_val_t("12Jack", 49));
  put_key_val(dict, create_key_val_t("Willow Tree", 87));

  return dict;
}

// TEST CODE BELOW

START_TEST(create_destroy_test) {
  dict_t *dict = create_dict();
  ck_assert_ptr_ne(dict, NULL);
  destroy_dict(dict);
}
END_TEST

START_TEST(create_insert_fetch_test) {
  dict_t *dict = create_dict();
  key_val_t *student = create_key_val_t("Johnson Caputo", 90);
  put_key_val(dict, student);

  // fetch the value to see if it works
  key_val_t *fetch_student = get_key_val(dict, student);
  ck_assert_str_eq(fetch_student->student_name, "Johnson Caputo");

  // destroy
  destroy_dict(dict);
}
END_TEST

START_TEST(create_insert_multi_test) {
  dict_t *dict = create_multi_dict();

  key_val_t *student = create_key_val_t("Willow Tree", 0);
  key_val_t *fetch_student = get_key_val(dict, student);

  ck_assert_str_eq(fetch_student->student_name, "Willow Tree");
  ck_assert(contains_key(dict, student));

  destroy_dict(dict);
  map_free_payload(student);
}
END_TEST

START_TEST(value_search) {
  dict_t *dict = create_multi_dict();
  test_value_search_t *value_payload = calloc(1, sizeof(*value_payload));
  value_payload->grade = 82;

  contains_value(dict, value_payload);

  ck_assert_str_eq(value_payload->payload->student_name, "Roger Williams");

  destroy_dict(dict);
  free(value_payload);
}
END_TEST

static TFun create_destroy_test_list[] = {create_destroy_test,
                                          create_insert_fetch_test, NULL};

static TFun search_dict_functions[] = {create_insert_multi_test, value_search,
                                       NULL};

static void add_tests(TCase *test_cases, TFun *test_functions) {
  while (*test_functions) {
    // add the test from the core_tests array to the t-case
    tcase_add_test(test_cases, *test_functions);
    test_functions++;
  }
}

Suite *tree_map_test_suite(void) {
  // suite creation
  Suite *bst_suite = suite_create("Tree Map Test Suite");
  TFun *test_list;
  TCase *test_cases;

  test_list = create_destroy_test_list;
  test_cases = tcase_create("Testing creation and deletion of object");
  add_tests(test_cases, test_list);
  suite_add_tcase(bst_suite, test_cases);

  test_list = search_dict_functions;
  test_cases = tcase_create("Testing fetching of objects");
  add_tests(test_cases, test_list);
  suite_add_tcase(bst_suite, test_cases);

  return bst_suite;
}
