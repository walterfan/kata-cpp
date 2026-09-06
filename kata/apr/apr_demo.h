#pragma once

#include <apr_errno.h>
#include <apr_general.h>
#include <apr_pools.h>

#include <stdexcept>
#include <string>

inline void check_apr(apr_status_t rv, const char *what) {
    if (rv == APR_SUCCESS) {
        return;
    }
    char buf[256];
    apr_strerror(rv, buf, sizeof(buf));
    throw std::runtime_error(std::string(what) + ": " + buf);
}

struct AprRuntime {
    AprRuntime() { check_apr(apr_initialize(), "apr_initialize"); }
    ~AprRuntime() { apr_terminate(); }

    AprRuntime(const AprRuntime &) = delete;
    AprRuntime &operator=(const AprRuntime &) = delete;
};

struct AprPool {
    apr_pool_t *p = nullptr;

    explicit AprPool(apr_pool_t *parent = nullptr) {
        check_apr(apr_pool_create(&p, parent), "apr_pool_create");
    }
    ~AprPool() {
        if (p) {
            apr_pool_destroy(p);
        }
    }

    AprPool(const AprPool &) = delete;
    AprPool &operator=(const AprPool &) = delete;

    apr_pool_t *get() const { return p; }
};
