#include <chessNet/server.h>
#include <iostream>
#include <chrono>  // for std::chrono::seconds

Server::Server(short port, std::function<void(const std::array<char, 1024>&)>&& callback)
    : io_context_(), acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), callback_(callback)
{
    std::cout << "Server: Starting..." << std::endl;
    active_sockets_ = {}; // Ensure empty size
    do_accept();
    worker_thread_ = std::thread([this]() { io_context_.run(); });

}

Server::~Server()
{
    std::cout << "Server: Stopping..." << std::endl;
    if (worker_thread_.joinable())
    {
        std::cout << "Server: Attempting to stop I/O context..." << std::endl;
        asio::steady_timer timer(io_context_);
        timer.expires_after(std::chrono::seconds(1));
        timer.async_wait([this](const std::error_code& /*ec*/) {
            std::cout << "Server: Force stopping I/O context..." << std::endl;
            io_context_.stop();
        });
        io_context_.run();
        std::cout << "Server: Joining worker thread..." << std::endl;
        worker_thread_.join();
        std::cout << "Server: Worker thread joined." << std::endl;
    }
}

void Server::close()
{
    std::cout << "Server: Closing..." << std::endl;
    io_context_.post([this]() { acceptor_.close(); });
}

void Server::send_to_all(const std::array<char, 1024>& msg)
{
    // std::cout << "Server: Preparing to send message to all clients..." << std::endl;
    for (auto& socket : active_sockets_)
    {
        asio::async_write(*socket, asio::buffer(msg), 
            [this](std::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    // std::cout << "Server: Message sent to all clients" << std::endl;
                }
                else
                {
                    std::cerr << "Write Error: " << ec.message() << std::endl;
                }
            });
    }
}

void Server::do_accept()
{
    // std::cout << "Server: Waiting for connection..." << std::endl;
    acceptor_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
    {
        if (!ec)
        {
            std::cout << "Server: Accepted connection from " << socket.remote_endpoint() << std::endl;
            auto shared_socket = std::make_shared<asio::ip::tcp::socket>(std::move(socket));
            {
                std::lock_guard<std::mutex> lock(connection_mutex_);
                active_sockets_.push_back(shared_socket);
                ++connection_count_;
            }
            connection_cv_.notify_one();
            auto buffer = std::make_shared<std::array<char, 1024>>();
            do_read(shared_socket, buffer);
        }
        else
        {
            // std::cerr << "Accept Error: " << ec.message() << std::endl;
        }

        do_accept();  // accept next connection
    });
}

void Server::wait_for_new_connection()
{
    std::unique_lock<std::mutex> lock(connection_mutex_);
    int current_count = connection_count_;
    connection_cv_.wait(lock, [this, current_count] { return connection_count_ > current_count; });
}

void Server::do_read(std::shared_ptr<asio::ip::tcp::socket> socket, std::shared_ptr<std::array<char, 1024>> buffer)
{
    asio::async_read(*socket, asio::buffer(*buffer),
        [this, socket, buffer](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                callback_(*buffer);
                do_read(socket, buffer);  // continue reading from this client
            }
            else if (ec != asio::error::eof)
            {
                std::cerr << "Read Error: " << ec.message() << std::endl;
                active_sockets_.remove(socket);  // remove socket if there was a read error
            }
        });
}