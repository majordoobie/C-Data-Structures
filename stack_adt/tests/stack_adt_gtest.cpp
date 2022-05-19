#include <gtest/gtest.h>

extern "C"
{
    #include <stack.h>
}


typedef struct payload_t
{
    int value;
} stack_payload_t;

class StackTestFixture :public ::testing::Test
{
 protected:
    void SetUp() override
    {
        stack = stack_init(free_payload);

        stack_push(stack, create_stack_payload(2));
        stack_push(stack, create_stack_payload(2));
        stack_push(stack, create_stack_payload(2));
        stack_push(stack, create_stack_payload(2));
        stack_push(stack, create_stack_payload(5));
        stack_push(stack, create_stack_payload(3));

    }
    void TearDown() override
    {
        stack_destroy(stack);
    }
 public:
    stack_adt_t * stack = nullptr;

    /*!
     * Mandatory function for the stack init
     * @param payload
     */
    static void free_payload(stack_payload_t * payload)
    {
        // only needed for testing
        if (NULL != payload)
        {
            free(payload);
        }
    }

    /*!
     * Mandatory function for creating the payloads that go in the stack
     * @param value
     * @return
     */
    static stack_payload_t * create_stack_payload(int value)
    {
        stack_payload_t * payload = (stack_payload_t *)calloc(1, sizeof
            (stack_payload_t));
        payload->value = value;
        return payload;
    }


};

TEST_F(StackTestFixture, TestPopValue)
{
    payload_t * payload = stack_pop(stack);
    EXPECT_EQ(payload->value, 3);
    free_payload(payload);
}

TEST_F(StackTestFixture, TestPeekValue)
{
    payload_t * payload = stack_peek(stack);
    EXPECT_EQ(payload->value, 3);
    EXPECT_EQ(stack_size(stack), 6);
}

TEST_F(StackTestFixture, TestPopAll)
{
    payload_t * payload;
    while(!stack_is_empty(stack))
    {
        payload = stack_pop(stack);
        free_payload(payload);
    }
}

TEST_F(StackTestFixture, TestFetchNthItem)
{
    payload_t * payload;
    payload = stack_nth_peek(stack, 3);

    // Using assert for safety
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(payload->value, 2);

    payload = stack_nth_peek(stack, -3);
    // Using assert for safety
    ASSERT_EQ(payload, nullptr);


    payload = stack_nth_peek(stack, stack_size(stack) + 10);
    // Using assert for safety
    ASSERT_EQ(payload, nullptr);
}

TEST_F(StackTestFixture, DumpStack)
{
    stack_dump(stack);
    EXPECT_EQ(stack_size(stack), 0);
}









