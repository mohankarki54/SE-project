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

int num1 = 0;
int r_value = 2;
bool split_v = false;
float temp_bet;

std::vector<char*> card_fir;


std::vector<char*> user_cards_f;
std::vector<char*> dealers_cards_f;

std::vector<char*> user_cards_s;
std::vector<char*> dealers_cards_s;


float play_credit;

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
        //  std::cout << "Outside if" << '\n';
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
          //  std::cout << "Inside if" << '\n';
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
            if(read_msg_.ca.stand != true){
              if(read_msg_.ca.track_num ==0 || read_msg_.ca.r_value == 1){
                if(read_msg_.ca.r_value == 1){
                  user_cards_f.clear();
                  user_cards_s.clear();
                  dealers_cards_f.clear();
                  dealers_cards_s.clear();
                  card_fir.clear();
                }
               r_value++;
               card_fir.push_back(read_msg_.ca.d1_face);
               std::cout << "-----------------------------------" << '\n';
               std::cout << "Your first card " << read_msg_.ca.c1_face << " of "<< read_msg_.ca.c1_suit<< std::endl;
               std::cout << "Your second card " << read_msg_.ca.c2_face << " of "<< read_msg_.ca.c2_suit<< std::endl;
               std::cout << "Dealer first card " << read_msg_.ca.d1_face << " of "<< read_msg_.ca.d1_suit<< std::endl;
               std::cout << "----------------------------------------" << '\n';

               user_cards_f.push_back(read_msg_.ca.c1_face);
               user_cards_s.push_back(read_msg_.ca.c2_suit);

               dealers_cards_f.push_back(read_msg_.ca.d1_face);
               dealers_cards_s.push_back(read_msg_.ca.d2_suit);
              }
            else if(read_msg_.ca.split){
              split_v = true;
              std::cout << "-----------Card is splitted-------------" << '\n';
              std::cout << "-------------------------------------------" << '\n';
              std::cout << "Your card " << read_msg_.ca.c1_face << " of "<< read_msg_.ca.c1_suit<< std::endl;
              std::cout << "Your card " << read_msg_.ca.c2_face << " of "<< read_msg_.ca.c2_suit<< std::endl;
              std::cout << "-------------------------------------------" << '\n';
            }
             else if(read_msg_.ca.hit){
               user_cards_f.push_back(read_msg_.ca.c1_face);
               user_cards_s.push_back(read_msg_.ca.c1_suit);
               std::cout << "-------------------------------------------" << '\n';
               std::cout << "Your card " << read_msg_.ca.c1_face << " of "<< read_msg_.ca.c1_suit<< std::endl;
               std::cout << "-------------------------------------------" << '\n';
             }
             }
             else{
          /*   int ss = user_cards_f.size();
   						std::cout << "---------------------------------" << '\n';
   					std::cout << "Your Cards" << '\n';
   						for(int i = 0; i < ss; i++){
   							std::cout << user_cards_f[i]<<" "<< user_cards_s[i] << '\n';
   						};*/
              std::cout << "Your card value: " << read_msg_.gs.p_value << '\n';
   						std::cout << "---------------------------------" << '\n';
   					//	std::cout << "Dealer Cards" << '\n';
   						/*int ss1 = dealers_cards_f.size();
   						for(int i = 0; i < ss1; i++){
   							std::cout << dealers_cards_f[i]<<" "<< dealers_cards_s[i] << '\n';
   						}*/
              std::cout << "Dealer card value: " << read_msg_.gs.d_value << '\n';
   						std::cout << "--------------------------------------" << '\n';
             }
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
            if(read_msg_.gs.tip){
              exit(0);
            }
            if (read_msg_.gs.valid && read_msg_.ca.stand != true){
              std::cout << "/* -------------------------------- */" << '\n';
              std::cout << "/* What you like to do? */" << '\n';
              std::cout << "Enter Hit for Hit" << '\n';
              std::cout << "Enter Stand for Stand or to deal." << '\n';
              std::cout << "Enter Split for Split" << '\n';
              std::cout << "Enter Insurance for Insurance" << '\n';
              std::cout << "Enter New to start the new game. *Game should be over*" << '\n';
              std::cout << "Enter Exit leave the game. *Game should be over*" << '\n';
              std::cout << "/* -------------------------------- */" << '\n';
            }
            else{
              play_credit = read_msg_.gs.players_credit;
              std::cout << "You have now $" << read_msg_.gs.players_credit << '\n';
              int cre = read_msg_.gs.players_credit - 100;
              if(cre < 0){
                int dd = -1 * cre;
                std::cout << "Sorry, You are Losing by $"<< dd << '\n';
              }
              else if(cre > 0){
                std::cout << "Congrtz, You winning by $" << cre << '\n';
              }
              else{
                std::cout << "Currently, you are even out." << '\n';
              }
            }

            //std::cout.write(read_msg_.body(), read_msg_.body_length());
            //std::cout << "\n";
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
              std::cout << "Not empty !!!" << '\n';
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

float send_betamount(){
  char dollar[chat_message::max_body_length + 1];
  std::cout << "/-----------------------------/" << '\n';
  std::cout << "Enter the bet amount: $";
  std::cin.getline(dollar, chat_message::max_body_length + 1);
  std::string fs(dollar);
  float dol =std::stof(fs);
  if(dol < 1  || dol > 5){
    std::cout << "----------------------" << '\n';
    std::cout << "*-------Error--------*" << '\n';
    std::cout << "Max Bet Amount is $5 and Min Bet is $1." << '\n';
    send_betamount();
  }
    return dol;
}

int startgame(){
  char ans[chat_message::max_body_length + 1];
  std::cout << "Do you like to start the game? ";
  std::cin.getline(ans, chat_message::max_body_length + 1);
  int result = strcmp(ans, "yes");
  if(result !=0){
    startgame();
  }
  return result;
}

float get_insurance(){
  char amo[chat_message::max_body_length + 1];
  std::cout << "Enter the insurance amount $";
  std::cin.getline(amo, chat_message::max_body_length + 1);
  float amo_ = stof(amo);
  float half = temp_bet/2;
  if(!(amo_ <= half)){
    get_insurance();
  }
  return half;
}


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

    char nam[chat_message::max_body_length + 1];

    chat_message msg;
    std::cout << "Welcome to the BlackJack Game" << '\n';

    int result = startgame();
    if(result == 0){
      msg.ca.start_game = true;
    }

    std::cout << "Enter your name: ";
    while (std::cin.getline(nam, chat_message::max_body_length + 1))
    {
      msg.ca.bet = true;
      msg.ca.track_num = num1;
      msg.ca.r_value = r_value;
      if(num1 == 0 ){
        msg.ca.name_valid = true;
        strcpy(msg.ca.name, nam);
        num1++;
    }else{
      if(strcmp(nam, "Hit")==0){
        msg.ca.hit = true;
        msg.ca.stand = false;
        msg.ca.insurance = false;

        if(split_v){
          msg.ca.split = true;
        }else{
          msg.ca.split = false;
        }

        msg.ca.bet = false;
        msg.ca.new_round = false;
        msg.gs.tip = false;

      }
      else if(strcmp(nam, "Stand")==0){
        char an[chat_message::max_body_length + 1];
        std::cout << " Are you sure you want to stand? ";
        std::cin.getline(an, chat_message::max_body_length + 1);
        if(strcmp(an, "yes") == 0){
          msg.ca.hit = false;
          msg.ca.stand = true;
          msg.ca.insurance =false;
          msg.ca.split =false;
          msg.ca.bet = false;
          msg.ca.new_round = false;
          msg.gs.tip = false;
        }
      }
      else if(strcmp(nam, "Split")==0){
      //  std::cout << " I am to Split" << '\n';
        split_v = true;
        msg.ca.hit = false;
        msg.ca.stand =false;
        msg.ca.insurance =false;
        msg.ca.split = true;
        msg.ca.bet = false;
        msg.ca.new_round = false;
        msg.gs.tip = false;

      }
      else if(strcmp(nam, "Insurance")==0){
        msg.ca.hit = false;
        msg.ca.stand =false;
        if(split_v){
          msg.ca.split = true;
        }else{
          msg.ca.split = false;
        }
        msg.ca.bet = false;
        msg.ca.new_round = false;
        msg.gs.tip = false;
        if(strcmp(card_fir[0], "Ace")==0){
          float amo_ = get_insurance();
          msg.ca.ins_amount = amo_;
          msg.ca.insurance = true;
        }
        else{
          std::cout << "--------------------------------------------" << '\n';
          std::cout << "Sorry, the dealers cards need to be Ace. You cannot have insurance." << '\n';
          std::cout << "--------------------------------------------" << '\n';
          msg.ca.insurance = false;
        }

      }
      else if(strcmp(nam, "New")==0){
      //  std::cout << " I am to Insurance" << '\n';
        msg.ca.hit = false;
        msg.ca.stand =false;
        msg.ca.insurance = false;
        msg.ca.split =false;
        msg.ca.bet = true;
        msg.ca.new_round = true;
        msg.gs.tip = false;

      }
      else if(strcmp(nam, "Exit")==0){
        msg.ca.hit = false;
        msg.ca.stand =false;
        msg.ca.insurance = false;
        msg.ca.split =false;
        msg.ca.bet = false;
        msg.ca.new_round = false;
        msg.gs.tip = true;
        if(play_credit>0){
          char amo[chat_message::max_body_length + 1];
          std::cout << "Enter the tip amount $";
          std::cin.getline(amo, chat_message::max_body_length + 1);
          float amo_ = stof(amo);
          msg.gs.tip_amount = amo_;
          std::cout << "Thank you so much for your Tip" << '\n';
        }else{
          exit(0);
        }

      }
      else{
        msg.gs.tip = false;
        msg.ca.hit = false;
        msg.ca.stand =false;
        msg.ca.insurance = false;
        msg.ca.split =false;
        msg.ca.bet = false;
        msg.ca.new_round = false;
      }
    }
    if(msg.ca.bet){
      float d = send_betamount();
      msg.ca.bet_amo_ = d;
      temp_bet = d;
      if(msg.ca.stand == true){
        msg.ca.stand = false;
        split_v = false;
        std::cout << "/*-------Starting the new round-------*/" << '\n';
        std::cout << '\n';
      }
    }

      //get the line from command and encode it.
      msg.body_length(std::strlen(nam));
      std::memcpy(msg.body(), nam, msg.body_length());
      msg.encode_header();
    //  std::cout << "Write here in main." << '\n';
      c.write(msg);

    }
    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
