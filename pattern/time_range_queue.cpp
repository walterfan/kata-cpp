#include <iostream>
#include <deque>
#include <chrono>
#include <ctime>
#include <vector>

using namespace std;

class TimeRangeQueue {
private:
    deque<pair<int, time_t>> q; // Queue container of pairs
    int max_length; // Maximum number of items in the queue
    time_t time_range; // Time range in seconds
public:
    TimeRangeQueue(int length, int range) : max_length(length), time_range(range) {}

    // Function to add an item to the queue
    void push(int data) {
        // Remove any items that are outside the time range
        time_t current_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        while (!q.empty() && current_time - q.front().second >= time_range) {
            q.pop_front();
        }

        // Add the new item to the back of the queue
        q.emplace_back(data, current_time);

        // Remove the oldest item if the queue is full
        if (q.size() > max_length) {
            q.pop_front();
        }
    }

    // Function to print the contents of the queue
    void print() {
        for (auto& item : q) {
            cout << item.first << " ";
        }
        cout << endl;
    }

    // Function to get a list of items within a specified time range
    vector<int> getRange(int range_start, int range_end) {
        vector<int> range_list;
        time_t current_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        time_t start_time = current_time - range_start;
        time_t end_time = current_time - range_end;

        for (auto& item : q) {
            if (item.second >= start_time && item.second <= end_time) {
                range_list.push_back(item.first);
            }
        }

        return range_list;
    }
};

int main() {
    TimeRangeQueue queue(5, 10); // Create a queue with a maximum length of 5 and time range of 10 seconds

    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);
    queue.print(); // Output: 1 2 3 4 5

    // Wait for 11 seconds to simulate the time range expiring
    this_thread::sleep_for(chrono::seconds(11));

    queue.push(6);
    queue.print(); // Output: 2 3 4 5 6

    // Get a list of items within the last 5 seconds
    vector<int> range_list = queue.getRange(5, 0);
    cout << "Items within the last 5 seconds: ";
    for (auto& item : range_list) {
        cout << item << " ";
    }
    cout << endl;

    return 0;
}
