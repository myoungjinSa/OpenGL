#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace Concurrent {
	template<class T>
	class SafeQueue {
	public:
		SafeQueue(void)
			: q()
			, m()
			, c()
		{}

		~SafeQueue() {

		}
		void Enqueue(T t) {
			std::lock_guard<std::mutex> lock(m);
			q.push(t);
			c.notify_one();
		}

		void Enqueue(T&& t) {
			std::lock_guard<std::mutex> lock(m);
			q.push(std::forward<T>(t));
			c.notify_one();
		}

		T Dequeue(void) {
			std::unique_lock<std::mutex> lock(m);

			while (q.empty()) {

				//release lock as long as the wait and reaquire it afterwards.
				c.wait(lock);
			}

			T val = q.front();
			q.pop();
			return val;
		}

		void Clear() {
			std::unique_lock<std::mutex> lock(m);
			
			while (!q.empty()) {
				T value;
				Try_pop(value);
			}
		}

		bool Try_pop(T& value) {
			std::unique_lock<std::mutex> lock(m);
			if (!c.wait_for(lock, std::chrono::microseconds{ 1 }, [this] { return !q.empty(); })) {
				return false;
			}
			value = std::move(q.front());
			q.pop();
			return true;
		}

	private:
		std::queue<T> q;
		mutable std::mutex m;
		std::condition_variable c;
	};
}