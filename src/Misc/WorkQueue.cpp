#include "WorkQueue.h"

namespace Tristeon
{
	namespace Misc
	{
		WorkQueue::WorkQueue()
		{
			num_threads = std::thread::hardware_concurrency();

			should_close = false;

			for (size_t i = 0; i < num_threads; i++)
			{
				pool.push_back(std::thread(&WorkQueue::run_work_loop, this));
			}
		}

		WorkQueue::~WorkQueue()
		{
			finish_jobs();
			should_close = true;
			jobs = {};
			running_jobs = {};
			condition.notify_all();
			for (auto& t : pool)
				t.join();
		}

		void WorkQueue::add_job(std::function<void(void*)> func, void* userData)
		{
			{
				std::lock_guard<std::mutex> lock(job_lock);
				jobs.push(Job(func, userData));
			}

			condition.notify_one();
		}

		void WorkQueue::run_work_loop()
		{
			while (true)
			{
				if (should_close)
					return;

				Job job;
				{
					std::unique_lock<std::mutex> job_guard(job_lock);
					condition.wait(job_guard, [&] { return !jobs.empty() || should_close; });

					if (should_close)
						return;

					job = jobs.front();
					jobs.pop();
				}

				{
					std::lock_guard<std::mutex> run_guard(running_lock);
					running_jobs.push_back(&job);
				}

				job.func(job.userData);

				{
					std::lock_guard<std::mutex> run_guard(running_lock);
					running_jobs.erase(std::find(running_jobs.begin(), running_jobs.end(), &job));
				}
			}
		}

		void WorkQueue::finish_jobs()
		{
			while (true)
			{
				std::lock_guard<std::mutex> lock(job_lock);
				std::lock_guard<std::mutex> lock2(running_lock);
				if (jobs.empty() && running_jobs.empty())
				{
					return;
				}
			}
		}

		bool WorkQueue::finished()
		{
			std::lock_guard<std::mutex> lock(job_lock);
			return jobs.empty();
		}

		int WorkQueue::count_threads() const
		{
			return num_threads;
		}
	}
}
