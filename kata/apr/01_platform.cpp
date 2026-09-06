// Cross-platform APR bootstrap: initialize the library and print OS/runtime info.
#include "apr_demo.h"

#include <apr_network_io.h>
#include <apr_version.h>

#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        apr_version_t ver{};
        apr_version(&ver);
        std::cout << "APR runtime " << ver.major << "." << ver.minor << "." << ver.patch
                  << " (" << apr_version_string() << ")\n";
        std::cout << "APR compile-time " << APR_MAJOR_VERSION << "." << APR_MINOR_VERSION
                  << "." << APR_PATCH_VERSION << "\n";

        char host[APRMAXHOSTLEN] = {};
        check_apr(apr_gethostname(host, sizeof(host), pool.get()), "apr_gethostname");
        std::cout << "hostname: " << host << "\n";

#if defined(DARWIN)
        std::cout << "platform define: DARWIN\n";
#elif defined(WIN32)
        std::cout << "platform define: WIN32\n";
#else
        std::cout << "platform define: POSIX-like\n";
#endif
        std::cout << "threads: " << (APR_HAS_THREADS ? "yes" : "no") << "\n";
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
