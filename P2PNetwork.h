#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <atomic>

class P2PNetwork {
public:
    using MessageCallback = std::function<void(const std::wstring&)>;
    using ConnectionCallback = std::function<void()>;
    std::function<void()> on_disconnected_;

    P2PNetwork();
    ~P2PNetwork();

    void setOnMessageReceived(MessageCallback callback);
    void setOnConnected(ConnectionCallback callback);
    void setOnDisconnected(std::function<void()> callback);

    void startHost(unsigned short port);
    void connectToHost(const std::wstring& host_ip, unsigned short port);
    void sendText(const std::wstring& text);
    void stop();
    void disconnect();
    bool isConnected() const;

private:
    void startReading();

    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    std::thread network_thread_;

    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;

    MessageCallback on_message_received_;
    ConnectionCallback on_connected_;
    std::atomic<bool> is_connected_{ false };
    uint32_t inbound_data_size_ = 0;
    std::vector<uint8_t> inbound_data_;
    void handleDisconnect(const boost::system::error_code& ec);
};