#pragma once

#include <asio.hpp>
#include <deque>
#include <string>
#include <memory>
#include <condition_variable>

class Client : public std::enable_shared_from_this<Client>  // inherit from std::enable_shared_from_thi
{
public:
    Client(const std::string& host, const std::string& port, std::function<void(const std::array<char, 1024>&)> on_message);
    ~Client();

    void write(const std::array<char, 1024>& msg);
    void close();

    void wait_for_message();

private:
    void do_connect(const asio::ip::tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();

    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    std::deque<std::array<char, 1024>> write_msgs_;
    std::array<char, 1024> read_msg_;
    std::thread worker_thread_;
    std::function<void(const std::array<char, 1024>&)> on_message_;

    // For wait for message
    std::condition_variable message_cv_;
    std::mutex message_mutex_;
    bool message_received_ = false;
};