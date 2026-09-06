// Character encoding: APR-util xlate (iconv) converts UTF-8 to ISO-8859-1.
#include "apr_demo.h"

#include <apr_xlate.h>

#include <cstdio>
#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        apr_xlate_t *conv = nullptr;
        check_apr(apr_xlate_open(&conv, "ISO-8859-1", "UTF-8", pool.get()), "apr_xlate_open");

        const char utf8[] = "caf\xC3\xA9";  // café in UTF-8
        apr_size_t in_left = sizeof(utf8) - 1;
        char out[32] = {};
        apr_size_t out_left = sizeof(out) - 1;
        check_apr(apr_xlate_conv_buffer(conv, utf8, &in_left, out, &out_left),
                  "apr_xlate_conv_buffer");
        apr_size_t written = (sizeof(out) - 1) - out_left;
        std::cout << "utf8 cafe + U+00E9 -> latin1 bytes:";
        for (apr_size_t i = 0; i < written; ++i) {
            std::printf(" %02X", static_cast<unsigned char>(out[i]));
        }
        std::cout << "\n";

        apr_xlate_close(conv);
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
