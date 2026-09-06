// Network: loopback TCP echo with a client thread and accept in the main thread.
#include "apr_demo.h"

#include <apr_network_io.h>
#include <apr_thread_proc.h>
#include <apr_time.h>

#include <cstring>
#include <iostream>
#include <string>

namespace {

struct ClientJob {
    apr_port_t port = 0;
    char echo[16] = {};
    apr_size_t echo_len = 0;
};

void *APR_THREAD_FUNC client_fn(apr_thread_t *thread, void *data) {
    auto *job = static_cast<ClientJob *>(data);
    apr_sleep(100 * 1000);

    apr_pool_t *p = nullptr;
    apr_status_t rv = apr_pool_create(&p, nullptr);
    if (rv != APR_SUCCESS) {
        apr_thread_exit(thread, rv);
        return nullptr;
    }

    apr_sockaddr_t *peer = nullptr;
    rv = apr_sockaddr_info_get(&peer, "127.0.0.1", APR_INET, job->port, 0, p);
    if (rv != APR_SUCCESS) {
        apr_pool_destroy(p);
        apr_thread_exit(thread, rv);
        return nullptr;
    }

    apr_socket_t *sock = nullptr;
    rv = apr_socket_create(&sock, APR_INET, SOCK_STREAM, APR_PROTO_TCP, p);
    if (rv != APR_SUCCESS) {
        apr_pool_destroy(p);
        apr_thread_exit(thread, rv);
        return nullptr;
    }
    apr_socket_timeout_set(sock, 5 * APR_USEC_PER_SEC);
    rv = apr_socket_connect(sock, peer);
    if (rv != APR_SUCCESS) {
        apr_socket_close(sock);
        apr_pool_destroy(p);
        apr_thread_exit(thread, rv);
        return nullptr;
    }

    const char *msg = "ping";
    apr_size_t n = strlen(msg);
    rv = apr_socket_send(sock, msg, &n);
    if (rv == APR_SUCCESS) {
        n = strlen(msg);
        rv = apr_socket_recv(sock, job->echo, &n);
        job->echo_len = n;
    }
    apr_socket_close(sock);
    apr_pool_destroy(p);
    apr_thread_exit(thread, rv);
    return nullptr;
}

}  // namespace

int main() {
    try {
        AprRuntime runtime;
        AprPool pool;

        apr_sockaddr_t *sa = nullptr;
        check_apr(apr_sockaddr_info_get(&sa, "127.0.0.1", APR_INET, 0, 0, pool.get()),
                  "apr_sockaddr_info_get");

        apr_socket_t *listen = nullptr;
        check_apr(apr_socket_create(&listen, APR_INET, SOCK_STREAM, APR_PROTO_TCP, pool.get()),
                  "apr_socket_create listen");
        check_apr(apr_socket_opt_set(listen, APR_SO_REUSEADDR, 1), "APR_SO_REUSEADDR");
        check_apr(apr_socket_bind(listen, sa), "apr_socket_bind");
        check_apr(apr_socket_listen(listen, 8), "apr_socket_listen");

        apr_sockaddr_t *local = nullptr;
        check_apr(apr_socket_addr_get(&local, APR_LOCAL, listen), "apr_socket_addr_get");

        ClientJob job;
        job.port = local->port;
        std::cout << "listening on 127.0.0.1:" << job.port << "\n";

        apr_thread_t *th = nullptr;
        check_apr(apr_thread_create(&th, nullptr, client_fn, &job, pool.get()),
                  "apr_thread_create client");

        apr_socket_t *accepted = nullptr;
        check_apr(apr_socket_accept(&accepted, listen, pool.get()), "apr_socket_accept");
        apr_socket_timeout_set(accepted, 5 * APR_USEC_PER_SEC);

        char buf[16] = {};
        apr_size_t n = 4;
        apr_status_t rv = apr_socket_recv(accepted, buf, &n);
        if (rv != APR_SUCCESS && !(APR_STATUS_IS_EOF(rv) && n > 0)) {
            check_apr(rv, "server recv");
        }
        check_apr(apr_socket_send(accepted, buf, &n), "server send");
        apr_socket_close(accepted);
        apr_socket_close(listen);

        apr_status_t client_rv = APR_SUCCESS;
        check_apr(apr_thread_join(&client_rv, th), "apr_thread_join");
        check_apr(client_rv, "client thread");
        std::cout << "echo: " << std::string(job.echo, job.echo_len) << "\n";
        std::cout << "ok\n";
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
