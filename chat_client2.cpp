//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <string>
#include "asio.hpp"
#include "chat_message2.hpp"

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg)
  {
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

  void close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            //Coming form the server
          //  std::vector<card> car = read_msg_.ca.p_card;
        //  std::cout << "Your card is"<< car.get_face() <<" of "<< car.get_suit() << '\n';
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            if (read_msg_.ca.valid)
              std::cout << "it is valid" << std::endl;
            if (read_msg_.gs.valid)
              std::cout << "the dealer points are " << read_msg_.gs.dealer_points << std::endl;
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
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

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);

    std::thread t([&io_context](){ io_context.run(); });

    char ans[chat_message::max_body_length + 1];
    char dollar[chat_message::max_body_length + 1];
    char nam[chat_message::max_body_length + 1];

    chat_message msg;
    std::cout << "Welcome to the BlackJack Game" << '\n';
    std::cout << "Do you like to start the game?" << '\n';
    std::cin.getline(ans, chat_message::max_body_length + 1);

    int result = strcmp(ans, "yes");
    if(result==0){
      msg.ca.start_game = true;
    }
    else{
      msg.ca.start_game = false;
    }

    std::cout << "Enter your name: ";
    std::cin.getline(nam, chat_message::max_body_length + 1);

    if(nam != NULL){
      msg.ca.name_valid = true;
      strcpy(msg.ca.name, nam);
    }

    std::cout << "/-----------------------------/" << '\n';
    std::cout << "Enter the bet amount: $";
    std::cin >> dollar;

    std::string fs(dollar);
    float dol =std::stof(fs);

    if(msg.ca.start_game){
    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {

      //get the line from command and encode it.
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      //
      msg.ca.bet_amo_ = dol;
      msg.ca.valid = true;
      msg.ca.C[0] = 'A';
      msg.encode_header(); //enconded such that I can decode in server

      //If need to write in the same terminal
      c.write(msg);
    }

    c.close();
    t.join();
  }}
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
