// Threads and mutexes: two workers increment a shared counter.
#include "apr_demo.h"

#include <apr_thread_mutex.h>
#include <apr_thread_proc.h>

#include <iostream>

namespace {

struct Shared {
    apr_thread_mutex_t *mutex = nullptr;
    int counter = 0;
};

void *APR_THREAD_FUNC worker(apr_thread_t *thread, void *data) {
    auto *shared = static_cast<Shared *>(data);
    for (int i = 0; i < 10000; ++i) {
        apr_thread_mutex_lock(shared->mutex);
        ++shared->counter;
        apr_thread_mutex_unlock(shared->mutex);
    }
    apr_thread_exit(thread, APR_SUCCESS);
    return nullptr;
}

}  // namespace

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        Shared shared;
        check_apr(apr_thread_mutex_create(&shared.mutex, APR_THREAD_MUTEX_DEFAULT, pool.get()),
                  "apr_thread_mutex_create");

        apr_thread_t *t1 = nullptr;
        apr_thread_t *t2 = nullptr;
        check_apr(apr_thread_create(&t1, nullptr, worker, &shared, pool.get()), "apr_thread_create t1");
        check_apr(apr_thread_create(&t2, nullptr, worker, &shared, pool.get()), "apr_thread_create t2");

        apr_status_t st1 = APR_SUCCESS;
        apr_status_t st2 = APR_SUCCESS;
        check_apr(apr_thread_join(&st1, t1), "apr_thread_join t1");
        check_apr(apr_thread_join(&st2, t2), "apr_thread_join t2");
        check_apr(st1, "worker t1");
        check_apr(st2, "worker t2");

        std::cout << "counter=" << shared.counter << " (expected 20000)\n";
        std::cout << (shared.counter == 20000 ? "ok\n" : "mismatch\n");
        return shared.counter == 20000 ? 0 : 1;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
