// Memory: APR pools allocate, copy, and free as a unit.
#include "apr_demo.h"

#include <apr_strings.h>

#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        void *raw = apr_palloc(pool.get(), 64);
        if (!raw) {
            throw std::runtime_error("apr_palloc failed");
        }

        char *copied = apr_pstrdup(pool.get(), "hello from pool");
        char *formatted = apr_psprintf(pool.get(), "answer=%d", 42);
        std::cout << copied << ", " << formatted << "\n";

        AprPool child(pool.get());
        char *nested = apr_pstrcat(child.get(), "nested-", "pool", nullptr);
        std::cout << nested << "\n";

        apr_pool_clear(child.get());
        char *after_clear = apr_pstrdup(child.get(), "reused after clear");
        std::cout << after_clear << "\n";
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
