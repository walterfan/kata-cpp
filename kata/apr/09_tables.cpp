// Data structures: APR array, table (multimap), and hash.
#include "apr_demo.h"

#include <apr_hash.h>
#include <apr_strings.h>
#include <apr_tables.h>

#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;
        apr_pool_t *p = pool.get();

        apr_array_header_t *nums = apr_array_make(p, 4, sizeof(int));
        *static_cast<int *>(apr_array_push(nums)) = 1;
        *static_cast<int *>(apr_array_push(nums)) = 2;
        *static_cast<int *>(apr_array_push(nums)) = 3;
        std::cout << "array:";
        for (int i = 0; i < nums->nelts; ++i) {
            std::cout << " " << APR_ARRAY_IDX(nums, i, int);
        }
        std::cout << "\n";

        apr_table_t *headers = apr_table_make(p, 4);
        apr_table_set(headers, "Host", "localhost");
        apr_table_add(headers, "Accept", "text/plain");
        apr_table_add(headers, "Accept", "application/json");
        std::cout << "table Host=" << apr_table_get(headers, "Host") << "\n";
        const apr_array_header_t *elts = apr_table_elts(headers);
        auto *pairs = reinterpret_cast<const apr_table_entry_t *>(elts->elts);
        for (int i = 0; i < elts->nelts; ++i) {
            std::cout << "  " << pairs[i].key << ": " << pairs[i].val << "\n";
        }

        apr_hash_t *map = apr_hash_make(p);
        apr_hash_set(map, "alpha", APR_HASH_KEY_STRING, apr_pstrdup(p, "a"));
        apr_hash_set(map, "beta", APR_HASH_KEY_STRING, apr_pstrdup(p, "b"));
        std::cout << "hash count=" << apr_hash_count(map)
                  << " alpha=" << static_cast<const char *>(apr_hash_get(map, "alpha", APR_HASH_KEY_STRING))
                  << "\n";

        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
