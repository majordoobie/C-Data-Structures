#include <gtest/gtest.h>
#include <stack.h>

typedef struct payload_t
{
    int value;
} stack_payload_t;

class StackTestFixture :public ::testing::Test
{
 protected:
    void SetUp() override
    {
        stack = init_stack(free_payload);

        push_stack(stack, create_stack_payload(2));
        push_stack(stack, create_stack_payload(2));
        push_stack(stack, create_stack_payload(2));
        push_stack(stack, create_stack_payload(2));
        push_stack(stack, create_stack_payload(5));
        push_stack(stack, create_stack_payload(3));

    }
    void TearDown() override
    {
        destroy_stack(stack);
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
    payload_t * payload = pop_stack(stack);
    EXPECT_EQ(payload->value, 3);
    free_payload(payload);
}

TEST_F(StackTestFixture, TestPeekValue)
{
    payload_t * payload = peek_stack(stack);
    EXPECT_EQ(payload->value, 3);
}
