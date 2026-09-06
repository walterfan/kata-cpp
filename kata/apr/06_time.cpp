// Time and date: now, sleep, rfc822-style format, elapsed interval.
#include "apr_demo.h"

#include <apr_time.h>

#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        apr_time_t start = apr_time_now();
        apr_sleep(50 * 1000);  // microseconds
        apr_time_t end = apr_time_now();

        char stamp[APR_RFC822_DATE_LEN] = {};
        check_apr(apr_rfc822_date(stamp, start), "apr_rfc822_date");

        apr_time_exp_t exploded{};
        check_apr(apr_time_exp_lt(&exploded, start), "apr_time_exp_lt");

        char ctime_buf[APR_CTIME_LEN] = {};
        check_apr(apr_ctime(ctime_buf, start), "apr_ctime");

        std::cout << "rfc822: " << stamp << "\n";
        std::cout << "ctime:  " << ctime_buf << "\n";
        std::cout << "local:  " << (exploded.tm_year + 1900) << "-"
                  << (exploded.tm_mon + 1) << "-" << exploded.tm_mday << "\n";
        std::cout << "slept ~" << apr_time_as_msec(end - start) << " ms\n";
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
