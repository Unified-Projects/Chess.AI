#pragma once

#include <asio.hpp>
#include <string>
#include <functional>
#include <thread>
#include <list>
#include <condition_variable>

class Server
{
public:
    Server(short port, std::function<void(const std::array<char, 1024>&)>&& callback);
    ~Server();

    void close();
    void send_to_all(const std::array<char, 1024>& msg);

    // Wait for the first connection
    void wait_for_new_connection();

private:
    void do_accept();
    void do_read(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<std::array<char, 1024>> buffer);

    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::function<void(const std::array<char, 1024>&)> callback_;
    std::thread worker_thread_;
    std::list<std::shared_ptr<asio::ip::tcp::socket>> active_sockets_;

    std::condition_variable connection_cv_;
    std::mutex connection_mutex_;
    int connection_count_ = 0;
};