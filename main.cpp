#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <condition_variable>
#include <mutex>

void dailyTask(){
    std::cout << "Task executed at : " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
}

// get the std::chrono::time_point for the upcoming 12:01 AM
std::chrono::system_clock::time_point getMidnight() {
    std::chrono::time_point now         = std::chrono::system_clock::now();
    std::time_t             currTime    = std::chrono::system_clock::to_time_t(now);
    std::tm                 localTime   = *std::localtime(&currTime);

    localTime.tm_hour   = 0;
    localTime.tm_min    = 1;
    localTime.tm_sec    = 0;

    std::chrono::time_point nextExecutionTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&localTime));
    if(nextExecutionTimePoint <= now) 
        nextExecutionTimePoint += std::chrono::minutes(15);
    std::cout << "Current time : " << std::chrono::system_clock::to_time_t(now) << std::endl;
    std::cout << "Next Task at : " << std::chrono::system_clock::to_time_t(nextExecutionTimePoint) << std::endl;

    return nextExecutionTimePoint;
}

// get the std::chrono::time_point for the time 15 seconds ahead
std::chrono::system_clock::time_point getFifteenSecondsAhead() {
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::chrono::time_point nextExecutionTimePoint = now + std::chrono::seconds(15);

    std::cout << "Current time : " << std::chrono::system_clock::to_time_t(now) << std::endl;
    std::cout << "Next Task at : " << std::chrono::system_clock::to_time_t(nextExecutionTimePoint) << std::endl;

    return nextExecutionTimePoint;
}

int main()
{
    std::mutex mut;
    std::condition_variable cond;
    std::thread workerThread([&](){
        while(true)
        {
            std::chrono::system_clock::time_point nextExecutionTime = getFifteenSecondsAhead();
            // std::chrono::system_clock::time_point nextExecutionTime = getMidnight();
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::unique_lock<std::mutex> lock(mut);

            cond.wait_until(
                lock, nextExecutionTime, [&](){
                    return std::chrono::system_clock::now() >= nextExecutionTime;
                }
            );
            dailyTask();
        } 
    });
    workerThread.join();
    return 0;
}