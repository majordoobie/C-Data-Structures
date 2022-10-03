#include <errno.h>
#include <stdio.h>

#include <utils.h>

/*!
 * @brief Verify if the pointer passed in is NULL. If the pointer is NULL then
 * return UV_INVALID_ALLOC and set errno value to `ENOMEM`.
 *
 * @param ptr Pointer to newly allocated memory
 * @return UV status indicating if successful or failure allocation
 */
util_verify_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        debug_print("%s", "[!] Unable to allocate memory");
        errno = ENOMEM;
        return UV_INVALID_ALLOC;
    }
    return UV_VALID_ALLOC;
}
