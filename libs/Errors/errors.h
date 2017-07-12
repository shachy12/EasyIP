#ifndef ERRORS_H
#define ERRORS_H

#define IF_FALSE_GOTO(__validate_value, __label) \
    do { \
        if (!(__validate_value)) { \
            goto __label; \
        } \
    } while(0)

#define IF_TRUE_GOTO(__validate_value, __label) IF_FALSE_GOTO(!(__validate_value), __label)

#define ON_NULL_GOTO(__validate_value, __label) IF_TRUE_GOTO(NULL == __validate_value, __label)

#endif
