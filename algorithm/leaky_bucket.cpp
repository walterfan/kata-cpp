#include <chrono>
#include <iostream>
#include <thread>

class LeakyBucket {
public:
  LeakyBucket(int capacity, int leak_rate) :
      capacity_{capacity},
      leak_rate_{leak_rate},
      tokens_{capacity_}
  {}

  bool request(int tokens) {
    // Leak tokens based on elapsed time
    auto now = std::chrono::steady_clock::now();
    int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_leak_).count();
    int leak_amount = elapsed_ms * leak_rate_ / 1000;
    tokens_ = std::max(0, tokens_ - leak_amount);
    last_leak_ = now;

    // Check if there are enough tokens in the bucket
    if (tokens <= tokens_) {
      tokens_ -= tokens;
      return true;
    } else {
      return false;
    }
  }

private:
  int capacity_;
  int leak_rate_;
  int tokens_;
  std::chrono::steady_clock::time_point last_leak_ = std::chrono::steady_clock::now();
};

int main() {
  LeakyBucket bucket{10, 1};  // Bucket with capacity 10 and leak rate of 1 token per second

  // Make 20 requests, with a delay of 500ms between each request
  for (int i = 0; i < 20; ++i) {
    if (bucket.request(1)) {
      std::cout << "Request " << i << " granted" << std::endl;
    } else {
      std::cout << "Request " << i << " denied" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  return 0;
}
