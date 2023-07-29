#include <chessNet/client.h>
#include <iostream>
#include <chrono>
#include <thread>

#include <iostream>

Client::Client(const std::string& host, const std::string& port, std::function<void(const std::array<char, 1024>&)> on_message)
    : io_context_(), socket_(io_context_), on_message_(on_message)
{
    std::cout << "Client: Connecting to " << host << ":" << port << "..." << std::endl;
    asio::ip::tcp::resolver resolver(io_context_);
    do_connect(resolver.resolve(host, port));

    worker_thread_ = std::thread([this]() { io_context_.run(); });
}

Client::~Client()
{
    std::cout << "Client: Disconnecting..." << std::endl;
    if (worker_thread_.joinable())
    {
        io_context_.stop();
        worker_thread_.join();
    }
}

void Client::write(const std::array<char, 1024>& msg)
{
    // std::cout << "Client: Preparing to write message..." << std::endl;

    asio::post(io_context_,
        [this, msg]()
        {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(msg);
            if (!write_in_progress)
            {
                do_write();
            }
        });
}

void Client::close()
{
    std::cout << "Client: Closing connection..." << std::endl;
    io_context_.post([this]() { socket_.close(); });
}

void Client::wait_for_message()
{
    std::unique_lock<std::mutex> lock(message_mutex_);
    message_cv_.wait(lock, [this] { return message_received_; });
    message_received_ = false;  // reset the flag for next time
}

void Client::do_connect(const asio::ip::tcp::resolver::results_type& endpoints)
{
    asio::async_connect(socket_, endpoints,
        [this, endpoints](std::error_code ec, const asio::ip::tcp::endpoint&)
        {
            if (!ec)
            {
                std::cout << "Client: Connected." << std::endl;
                do_read();
            }
            else
            {
                std::cerr << "Connect Error: " << ec.message() << "\n";
                std::cerr << "Retry in 3 seconds...\n";

                // Wait for 3 seconds before retrying
                asio::steady_timer timer(io_context_);
                timer.expires_after(std::chrono::seconds(3));
                timer.async_wait([this, endpoints](std::error_code /*ec*/)
                {
                    // Retry the connection
                    do_connect(endpoints);
                });
            }
        });
}

void Client::do_read()
{
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(read_msg_),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                on_message_(read_msg_);
                {
                    std::lock_guard<std::mutex> lock(message_mutex_);
                    message_received_ = true;
                }
                message_cv_.notify_one();  // unblock wait_for_message()
                do_read();
            }
            else
            {
                socket_.close();
            }
        });
}

void Client::do_write()
{
    // std::cout << "Client: Attempting to write message..." << std::endl;
    asio::async_write(socket_, asio::buffer(write_msgs_.front()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                write_msgs_.pop_front();
                if (!write_msgs_.empty())
                {
                    do_write();
                }
            }
            else
            {
                socket_.close();
            }
        });
}
