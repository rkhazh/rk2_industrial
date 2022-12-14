#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>

class PingPong
{
public:
	static constexpr std::size_t MAX = 3;
    PingPong(): count_(0){};

	void ping()
	{
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock); // нам необходимо, чтобы ping был первым всегда
    	while (count_.load() < MAX)
    	{
        	std::cout << "Ping" << std::endl;
        	count_++;
        	cv_.notify_all();
            cv_.wait(lock);
    	}
        cv_.notify_all(); // Чтобы "разморозить" pong
 	}

	void pong()
	{
    	std::unique_lock<std::mutex> lock(m_);
        cv_.notify_all(); // обозначаем пингу, что мы готовы 
        cv_.wait(lock);
    	while (count_.load() < MAX)
    	{
        	std::cout << "Pong" << std::endl;
        	count_++;
        	cv_.notify_all();
            cv_.wait(lock);
    	}
        cv_.notify_all(); // Чтобы "разморозить" ping
	}

private:
	std::atomic<std::size_t> count_;
	std::mutex m_;
	std::condition_variable cv_;
};

int main()
{
	PingPong p;
	std::thread pingThread(&PingPong::ping, &p);
	std::thread pongThread(&PingPong::pong, &p);

	pingThread.join();
	pongThread.join();
}
