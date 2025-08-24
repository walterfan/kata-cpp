#pragma once

#include <boost/asio/ssl.hpp>
#include <boost/circular_buffer.hpp>
#include <memory>


struct Apdex {
    uint32_t satisfied_count;
    uint32_t tolerating_count;
    uint32_t frustrated_count;
    uint32_t failure_count;
};

struct Workload {
    uint32_t connection_count;
    uint32_t pending_events;
    uint32_t thread_count;
    uint32_t cpu_usage;
    uint32_t mem_usage;
    uint32_t disk_usage;
};

class WorkloadReporterBase {
public:
    virtual ~WorkloadReporterBase() = default;

    virtual int reportWorkload(const Workload& load) = 0;

    virtual int reportApdex(const Apdex& apdex) = 0;

};

class WorkloadReporter: public WorkloadReporterBase {
public:

};