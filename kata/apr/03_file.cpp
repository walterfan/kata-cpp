// Files and directories: create a temp dir, write, read, stat, then clean up.
#include "apr_demo.h"

#include <apr_file_info.h>
#include <apr_file_io.h>
#include <apr_strings.h>

#include <cstring>
#include <iostream>

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        const char *tmp = nullptr;
        check_apr(apr_temp_dir_get(&tmp, pool.get()), "apr_temp_dir_get");

        const char *dir = apr_pstrcat(pool.get(), tmp, "/kata-apr-file-demo", nullptr);
        apr_status_t mk = apr_dir_make(dir, APR_FPROT_UREAD | APR_FPROT_UWRITE | APR_FPROT_UEXECUTE,
                                       pool.get());
        if (mk != APR_SUCCESS && !APR_STATUS_IS_EEXIST(mk)) {
            check_apr(mk, "apr_dir_make");
        }

        const char *path = apr_pstrcat(pool.get(), dir, "/note.txt", nullptr);
        apr_file_t *fp = nullptr;
        check_apr(apr_file_open(&fp, path, APR_FOPEN_CREATE | APR_FOPEN_WRITE | APR_FOPEN_TRUNCATE,
                                APR_FPROT_UREAD | APR_FPROT_UWRITE, pool.get()),
                  "apr_file_open write");

        const char *msg = "apr file io\n";
        apr_size_t n = strlen(msg);
        check_apr(apr_file_write(fp, msg, &n), "apr_file_write");
        check_apr(apr_file_close(fp), "apr_file_close");

        apr_finfo_t info{};
        check_apr(apr_stat(&info, path, APR_FINFO_SIZE, pool.get()), "apr_stat");
        std::cout << "wrote " << info.size << " bytes to " << path << "\n";

        check_apr(apr_file_open(&fp, path, APR_FOPEN_READ, APR_OS_DEFAULT, pool.get()),
                  "apr_file_open read");
        char buf[64] = {};
        n = sizeof(buf) - 1;
        apr_status_t rd = apr_file_read(fp, buf, &n);
        if (rd != APR_SUCCESS && !APR_STATUS_IS_EOF(rd)) {
            check_apr(rd, "apr_file_read");
        }
        apr_file_close(fp);
        std::cout << "read: " << buf;

        check_apr(apr_file_remove(path, pool.get()), "apr_file_remove");
        apr_dir_remove(dir, pool.get());
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
