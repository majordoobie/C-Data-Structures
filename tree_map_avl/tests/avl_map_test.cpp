#include <gtest/gtest.h>
#include <tree_map_avl.h>


/*!
 * This is the structure that your code is using. This can be anything
 * AS LONG AS THE STRUCT TAG IS "bst_node_payload_t". The tag name is used
 * internally and is what allows us to map the two structures
 */
typedef struct bst_node_payload_t
{
    char * student_name;
    double grade_avg;
} key_val_t;

typedef struct
{
    double grade;
    key_val_t * payload;
} test_value_search_t;

/*!
 * Required function to compare the different nodes. This is used to ensure
 * that the tree is build correctly.
 *
 * @param current_payload Node struct you created
 * @param new_payload The payload that is being compared
 * @return Comparison result
 */
bst_compare_t map_compare(key_val_t * current_payload, key_val_t * new_payload)
{
    int val = strcmp(current_payload->student_name, new_payload->student_name);
    if (0 == val)
    {
        return BST_EQ;
    }
    else if (val > 0)
    {
        return BST_LT;
    }
    else
    {
        return BST_GT;
    }
}

/*!
 * Required function to be passed to dict init to clean up after itself
 * @param node_payload
 */
void map_free_payload(key_val_t * node_payload)
{
    // only needed for testing
    if (nullptr != node_payload)
    {
        free(node_payload->student_name);
        free(node_payload);
    }
}

/*!
 * Required function to be passed to the dict init. This is the functions that
 * is going to be used by the map to fetch value pairs if searching for values
 *
 * @param node_payload  Node_payload is the payload you created
 * @param ptr Void pointer can be empty but should be used for storing the
 * result
 * @return If recurse should continue
 */
bst_recurse_t map_value_search(key_val_t * node_payload, void * ptr)
{
    test_value_search_t * match = (test_value_search_t *) ptr;
    if ((int) node_payload->grade_avg == (int) match->grade)
    {
        match->payload = node_payload;
        return RECURSE_STOP;
    }
    return RECURSE_TRUE;
}

/*!
 * Helper function to create the kay_value pairs for this testing suite
 */
key_val_t * create_key_val_t(const char * name, double avg)
{
    key_val_t * key_val = (key_val_t *)calloc(1, sizeof(key_val_t));
    key_val->student_name = strdup(name);
    key_val->grade_avg = avg;
    return key_val;
}

class AVLMapTestFixture :public ::testing::Test
{
 protected:
    void SetUp() override
    {
        dict = init_dict(map_compare, map_free_payload,
                                  map_value_search);

        put_key_val(dict, create_key_val_t("Johnson Caputo", 74));
        put_key_val(dict, create_key_val_t("johnson Caputo", 106));
        put_key_val(dict, create_key_val_t("Steven Solis", 83));
        put_key_val(dict, create_key_val_t("Roger Williams", 82));
        put_key_val(dict, create_key_val_t("Roger Carl", 81));
        put_key_val(dict, create_key_val_t("K", 75));
        put_key_val(dict, create_key_val_t("12Jack", 49));
        put_key_val(dict, create_key_val_t("Willow Tree", 87));


        // create single students for testing
        student_1 = create_key_val_t("Johnson Caputo", 20);
        student_2 = create_key_val_t("Willow Tree", 100);
        student_3 = create_key_val_t("Kyle Pecker", 3);
        student_4 = create_key_val_t("Unknown", 74);
    }
    void TearDown() override
    {
        destroy_dict(dict);
        map_free_payload(student_1);
        map_free_payload(student_2);
        map_free_payload(student_3);
        map_free_payload(student_4);

    }

 public:
    dict_t * dict;
    key_val_t * student_1;
    key_val_t * student_2;
    key_val_t * student_3;
    key_val_t * student_4;
};

TEST_F(AVLMapTestFixture, TestFetchingStudents)
{
    key_val_t * student = get_key_val(dict, student_1);
    // compare that the name is the same
    EXPECT_EQ(0, strcmp(student->student_name, student_1->student_name));
    // proof that the pointers are unique
    EXPECT_NE(student, student_1);
}
TEST_F(AVLMapTestFixture, TestContainsStudents)
{
    /*
     * Student 1 exists while student 3 does not exist
     */
    bool present = contains_key(dict, student_1);
    EXPECT_TRUE(present);

    present = contains_key(dict, student_3);
    EXPECT_FALSE(present);
}
TEST_F(AVLMapTestFixture, TestValueReplacement)
{
    double old_avg = (get_key_val(dict, student_1))->grade_avg;
    put_key_val(dict, student_1);

    double new_avg = (get_key_val(dict, student_1))->grade_avg;
    EXPECT_NE(old_avg, new_avg);

    student_1 = nullptr;
}
TEST_F(AVLMapTestFixture, TestContainsValue)
{
    test_value_search_t * match = (test_value_search_t *)calloc(1, sizeof
    (test_value_search_t));

    // Nothing matches this
    match->grade = 99;
    contains_value(dict, match);

    EXPECT_TRUE(match->payload == nullptr);

    // get a value that exists
    key_val_t * student = get_key_val(dict, student_1);
    match->grade = student->grade_avg;

    // search again with a value that definitely exists
    contains_value(dict, match);
    EXPECT_EQ(match->payload->grade_avg, student->grade_avg);

    free(match);
}
















