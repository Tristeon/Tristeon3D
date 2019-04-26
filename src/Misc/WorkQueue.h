#pragma once

#include "WorkQueue.h"
#include <queue>
#include <thread>
#include <condition_variable>

namespace Tristeon
{
	namespace Misc
	{
		struct Job
		{
			std::function<void(void*)> func;
			void* userData;
			Job() : func(nullptr), userData(nullptr) { }
			explicit Job(std::function<void(void*)> func, void* userData = nullptr) : func(func), userData(userData) { };
		};

		/**
		 * A threaded work queue. n amount of threads wait till they get a job to execute.
		 * Creates n amount of threads in where n = std::thread::hardware_concurrency().
		 */
		class WorkQueue
		{
		public:
			WorkQueue();
			~WorkQueue();
			/**
			 * Adds a job to the queue. The job will be executed when a thread's free.
			 * \param userData The user data will be passed on to the job call. This data does NOT get deleted, please delete manually if necessary.
			 */
			void add_job(std::function<void(void*)> func, void* userData);
			
			/**
			 * Freezes the current thread until all jobs are finished
			 */
			void finish_jobs();

			/**
			 * Returns true if all jobs are finished
			 */
			inline bool finished();

			/**
			 * Returns the amount of threads the queue is running
			 */
			int count_threads() const;
		private:
			std::queue<Job> jobs;
			std::vector<Job*> running_jobs;
			std::mutex job_lock;
			std::mutex running_lock;
			std::vector<std::thread> pool;
			std::condition_variable condition;
			int num_threads;
			bool should_close;

			void run_work_loop();
		};
	}
}
