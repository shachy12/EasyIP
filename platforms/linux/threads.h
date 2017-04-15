#ifndef LINUX_THREADS_H
#define LINUX_THREADS_H

typedef enum {
#define THREAD_HANDLER(__thread_id, __thread_function) \
    __thread_id,
#include <platforms/linux/threads.hx>
#undef THREAD_HANDLER
    THREAD_COUNT
} thread_id_t;

#endif
