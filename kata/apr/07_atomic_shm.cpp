// Atomics plus anonymous shared memory: two threads increment a shared word.
#include "apr_demo.h"

#include <apr_atomic.h>
#include <apr_shm.h>
#include <apr_thread_proc.h>

#include <iostream>

namespace {

struct Job {
    volatile apr_uint32_t *counter = nullptr;
};

void *APR_THREAD_FUNC bump(apr_thread_t *thread, void *data) {
    auto *job = static_cast<Job *>(data);
    for (int i = 0; i < 10000; ++i) {
        apr_atomic_inc32(job->counter);
    }
    apr_thread_exit(thread, APR_SUCCESS);
    return nullptr;
}

}  // namespace

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        apr_shm_t *shm = nullptr;
        check_apr(apr_shm_create(&shm, sizeof(apr_uint32_t), nullptr, pool.get()),
                  "apr_shm_create anonymous");
        auto *counter = static_cast<volatile apr_uint32_t *>(apr_shm_baseaddr_get(shm));
        apr_atomic_set32(counter, 0);

        Job job{counter};
        apr_thread_t *t1 = nullptr;
        apr_thread_t *t2 = nullptr;
        check_apr(apr_thread_create(&t1, nullptr, bump, &job, pool.get()), "thread 1");
        check_apr(apr_thread_create(&t2, nullptr, bump, &job, pool.get()), "thread 2");

        apr_status_t st1 = APR_SUCCESS;
        apr_status_t st2 = APR_SUCCESS;
        check_apr(apr_thread_join(&st1, t1), "join 1");
        check_apr(apr_thread_join(&st2, t2), "join 2");

        apr_uint32_t value = apr_atomic_read32(counter);
        std::cout << "atomic counter=" << value << " (expected 20000)\n";
        std::cout << "shm size=" << apr_shm_size_get(shm) << "\n";
        check_apr(apr_shm_destroy(shm), "apr_shm_destroy");
        std::cout << (value == 20000 ? "ok\n" : "mismatch\n");
        return value == 20000 ? 0 : 1;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
