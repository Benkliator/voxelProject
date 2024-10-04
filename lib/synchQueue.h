#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class synchQueue {
public:
        synchQueue() {}

        void push(T object) {
            std::lock_guard<std::mutex> lock(synchMutex);
            elements.push(object);
            synchCV.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(synchMutex);
            synchCV.wait(lock, 
                         [this] { return !elements.empty(); });

            T ret = elements.front();
            elements.pop();
            return ret;
        }

        T front() {
            std::lock_guard<std::mutex> lock(synchMutex);
            T ret = elements.front();
            return ret;
        }

        bool empty() {
            std::unique_lock<std::mutex> lock(synchMutex);
            return elements.empty();
        }

private:
        std::queue<T> elements;
        std::mutex synchMutex;
        std::condition_variable synchCV;
};
