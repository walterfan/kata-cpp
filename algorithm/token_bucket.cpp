#include <chrono>
#include <iostream>
#include <thread>

class TokenBucket {
public:
  TokenBucket(int capacity, int refill_rate) :
      capacity_{capacity},
      refill_rate_{refill_rate},
      tokens_{0}
  {}

  bool request(int tokens) {
    // Refill tokens based on elapsed time
    auto now = std::chrono::steady_clock::now();
    int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refill_).count();
    int refill_amount = elapsed_ms * refill_rate_ / 1000;
    tokens_ = std::min(capacity_, tokens_ + refill_amount);
    last_refill_ = now;

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
  int refill_rate_;
  int tokens_;
  std::chrono::steady_clock::time_point last_refill_ = std::chrono::steady_clock::now();
};

int main() {
  TokenBucket bucket{10, 1};  // Bucket with capacity 10 and refill rate of 1 token per second

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
