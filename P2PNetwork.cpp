#include "P2PNetwork.h"
#include <istream>
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

using boost::asio::ip::tcp;

P2PNetwork::P2PNetwork()
    : io_context_(),
    work_guard_(boost::asio::make_work_guard(io_context_)),
    socket_(io_context_)
{
    network_thread_ = std::thread([this]() {
        io_context_.run();
        });
}

P2PNetwork::~P2PNetwork() {
    stop();
}

bool P2PNetwork::isConnected() const {
    return is_connected_;
}

void P2PNetwork::setOnMessageReceived(MessageCallback callback) {
    on_message_received_ = callback;
}

void P2PNetwork::startHost(unsigned short port) {
    boost::asio::post(io_context_, [this, port]() {
        try {
            acceptor_ = std::make_unique<tcp::acceptor>(
                io_context_,
                tcp::endpoint(tcp::v6(), port)
            );

            acceptor_->async_accept(socket_, [this](boost::system::error_code ec) {
                if (!ec) {
                    startReading();
                    is_connected_ = true;

                    if (on_connected_) {
                        on_connected_();
                    }
                }
                else
                {
                    std::string error_msg = ec.message();
                    std::wstring w_error(error_msg.begin(), error_msg.end());
                    MessageBoxW(NULL, w_error.c_str(), L"Помилка підключення Boost", MB_ICONERROR);
                }
                });
        }
        catch (const std::exception& e) {
            std::string error_msg = e.what();
            std::wstring w_error(error_msg.begin(), error_msg.end());
            MessageBoxW(NULL, w_error.c_str(), L"Критична помилка Хоста", MB_ICONERROR);
        }
        });
}

void P2PNetwork::connectToHost(const std::wstring& host_ip, unsigned short port) {
    boost::asio::post(io_context_, [this, host_ip, port]() {
        try {
            tcp::resolver resolver(io_context_);
            std::string narrow_ip(host_ip.begin(), host_ip.end());

            auto endpoints = resolver.resolve(narrow_ip, std::to_string(port));

            boost::asio::async_connect(socket_, endpoints,
                [this](boost::system::error_code ec, const tcp::endpoint&) {
                    if (!ec) {
                        startReading();
                        is_connected_ = true;

                        if (on_connected_) {
                            on_connected_();
                        }
                    }
                    else {
                        std::string error_msg = ec.message();
                        std::wstring w_error(error_msg.begin(), error_msg.end());
                        MessageBoxW(NULL, w_error.c_str(), L"Помилка підключення Boost", MB_ICONERROR);
                    }
                }); 
        }
        catch (const std::exception& e) {
            std::string error_msg = e.what();
            std::wstring w_error(error_msg.begin(), error_msg.end());
            MessageBoxW(NULL, w_error.c_str(), L"Критична помилка Хоста", MB_ICONERROR);
        } 
        });
}

void P2PNetwork::setOnConnected(ConnectionCallback callback) {
    on_connected_ = callback;
}

void P2PNetwork::setOnDisconnected(std::function<void()> callback)
{
    on_disconnected_ = callback;
}

void P2PNetwork::sendText(const std::wstring& text) {
    if (text.empty()) {
        return;
    }

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
    if (size_needed <= 0) {
        return;
    }

    std::string utf8_text(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &utf8_text[0], size_needed, NULL, NULL);

    auto packet = std::make_shared<std::vector<uint8_t>>();

    uint32_t text_length = utf8_text.size();

    packet->resize(sizeof(uint32_t) + text_length);

    std::memcpy(packet->data(), &text_length, sizeof(uint32_t));

    std::memcpy(packet->data() + sizeof(uint32_t), utf8_text.data(), text_length);

    boost::asio::post(io_context_, [this, packet]() {
        if (!socket_.is_open()) {
            return;
        }

        boost::asio::async_write(socket_, boost::asio::buffer(*packet),
            [packet](boost::system::error_code ec, std::size_t) {
                
            });
        });
}

void P2PNetwork::disconnect() {
    boost::asio::post(io_context_, [this]() {
        if (socket_.is_open()) {
            boost::system::error_code ec;

            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

            socket_.close(ec);
            is_connected_ = false;
        }
        });
}

void P2PNetwork::stop() {
    boost::asio::post(io_context_, [this]() {
        if (acceptor_ && acceptor_->is_open()) acceptor_->close();
        if (socket_.is_open()) socket_.close();
        });

    work_guard_.reset();
    io_context_.stop();

    if (network_thread_.joinable()) {
        network_thread_.join();
    }
    is_connected_ = false;
}

void P2PNetwork::handleDisconnect(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::eof ||
        ec == boost::asio::error::connection_reset ||
        ec == boost::asio::error::connection_aborted ||
        ec == boost::asio::error::operation_aborted)
    {
        if (on_disconnected_) {
            on_disconnected_();
        }
    }
}

void P2PNetwork::startReading() {
    boost::asio::async_read(socket_, boost::asio::buffer(&inbound_data_size_, sizeof(inbound_data_size_)),
        [this](boost::system::error_code ec, std::size_t ) {

            if (!ec) {
                
                inbound_data_.resize(inbound_data_size_);

                boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
                    [this](boost::system::error_code ec, std::size_t ) {

                        if (!ec) {
                            
                            std::string message(inbound_data_.begin(), inbound_data_.end());

                            if (!message.empty()) {
                                
                                int size_needed = MultiByteToWideChar(CP_UTF8, 0, &message[0], (int)message.size(), NULL, 0);
                                std::wstring w_message(size_needed, 0);
                                MultiByteToWideChar(CP_UTF8, 0, &message[0], (int)message.size(), &w_message[0], size_needed);

                                
                                if (on_message_received_) {
                                    on_message_received_(w_message);
                                }
                            }

                            startReading();
                        }
                        else {
                            
                            handleDisconnect(ec);
                        }
                    });
            }
            else {
                
                handleDisconnect(ec);
            }
        });
}