#include <check.h>
#include <stdio.h>
#include <intersect.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>


START_TEST(payload_creation_test){
    setlocale(LC_ALL, "en_US.UTF-8");
    key_val_t * payload2 = create_intersect_payload(L"Jack", 1);
    key_val_t * payload = create_intersect_payload(L"Zebra", 1);

    intersect_free_payload(payload);
    intersect_free_payload(payload2);
}END_TEST

START_TEST(dict_create_test){
    intersect_t * intersect = create_intersect_dict();
    put_key_val(intersect->dict, create_intersect_payload(L"Jack", 1));
    put_key_val(intersect->dict, create_intersect_payload(L"Zebra", 1));
    put_key_val(intersect->dict, create_intersect_payload(L"Carillón", 1));
    put_key_val(intersect->dict, create_intersect_payload(L"Chávez", 1));
    put_key_val(intersect->dict, create_intersect_payload(L"Cañoso", 1));
    put_key_val(intersect->dict, create_intersect_payload(L"Côte", 1));

    freopen(NULL, "wb", stdout);

    key_val_t * source = create_intersect_payload(L"Cañoso", 1);
    key_val_t * target = get_key_val(intersect->dict, source);
    ck_assert_ptr_ne(NULL, target);

    key_val_t * source2 = create_intersect_payload(L"cañoSO", 1);
    target = get_key_val(intersect->dict, source2);

    ck_assert_ptr_ne(NULL, target);

    // check that the fetched (with different case) matches what was on disk
    if (wcscmp(target->original, source->original) == 0)
    {
        ck_assert(true);
    }
    else
    {
        ck_assert(false);
    }

    destroy_dict(intersect->dict);
    free(intersect);
    intersect_free_payload(source);
    intersect_free_payload(source2);
}END_TEST

START_TEST(dict_variation_test){
    intersect_t * intersect = create_intersect_dict();
    dict_t * dict = intersect->dict;

    put_key_val_adv(dict, create_intersect_payload(L"Jack", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Zebra", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Carillón", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Chávez", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Cañoso", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Côte", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"côTe", 1), intersect_adv_add, intersect);


    destroy_dict(dict);
    free(intersect);

}END_TEST

START_TEST(dict_deletion_test){
    intersect_t * intersect = create_intersect_dict();
    intersect->file_iteration = 1;
    dict_t * dict = intersect->dict;

    put_key_val_adv(dict, create_intersect_payload(L"Jack", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Zebra", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Carillón", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Chávez", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Cañoso", 1), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Côte", 1), intersect_adv_add, intersect);

    intersect->file_iteration = 2;
    put_key_val_adv(dict, create_intersect_payload(L"côTe", 2), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"cháVez", 2), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"cháVEZ", 2), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"CariLLón", 2), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"Api2_", 2), intersect_adv_add, intersect);

    // reduce to those that didn't make it
    intersect_reduce(dict, 2);

    intersect->file_iteration++;
    put_key_val_adv(dict, create_intersect_payload(L"cháVEz", 3), intersect_adv_add, intersect);
    put_key_val_adv(dict, create_intersect_payload(L"CaRiLLón", 3), intersect_adv_add, intersect);
    intersect_reduce(dict, 3);

    recurse_dict(dict, intersect_print, NULL);
    destroy_dict(dict);
    free(intersect);
}END_TEST

static TFun creation_default_test[] = {
    payload_creation_test,
    dict_create_test,
    dict_variation_test,
    dict_deletion_test,
    NULL
};

static void add_tests(TCase * test_cases, TFun * test_functions)
{
    while (* test_functions) {
        // add the test from the core_tests array to the t-case
        tcase_add_test(test_cases, * test_functions);
        test_functions++;
    }
}

Suite * intersect_test_suite(void)
{

    setlocale(LC_ALL, "en_US.UTF-8");

    // suite creation
    Suite * bst_suite = suite_create("Intersection Test Suite");
    TFun * test_list;
    TCase * test_cases;

    test_list = creation_default_test;
    test_cases = tcase_create("Testing creation and deletion of object");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    return bst_suite;
}
