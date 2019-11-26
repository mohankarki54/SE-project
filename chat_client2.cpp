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


std::vector<char*> user_cards_f;

/*---------------------------*/
int card_value(char* card){
	//Calculate the individual card value.
		if(strcmp(card, "Ace") == 0){
			return 1;
		}
		else if(strcmp(card, "2") == 0){
			return 2;
		}
		else if(strcmp(card, "3") == 0){
			return 3;
		}
		else if(strcmp(card, "4") == 0){
			return 4;
		}
		else if(strcmp(card, "5") == 0){
			return 5;
		}
		else if(strcmp(card, "6") == 0){
			return 6;
		}
		else if(strcmp(card, "7") == 0){
			return 7;
		}
		else if(strcmp(card, "8") == 0){
			return 8;
		}
		else if(strcmp(card, "9") == 0){
			return 9;
		}
		else{
			return 10;
		}
}

int is_ace(char* card ){
	//checks for is the card is ace or not. If yes, return 1 else 0
	int value;
	if(strcmp(card, "Ace") == 0){
		value =  1;
	}
	else{
		value = 0;
	}
	return value;
}

std::vector<int> calculate_hand_value (vector<char*> cards)
{
//When Ace there will be two different value, and returns the hald value of the user or dealer.
	int N = cards.size();
	std::vector<int> value;
	int val1 = 0;
	int val2 = 0;
	int i;
	for(i = 0; i< N; i++){
		if(is_ace(cards[i])){
			val1 += 1;
			val2 += 11;
		}else{
			val1 += card_value(cards[i]);
			val2 += card_value(cards[i]);
		}
	}
	value.push_back(val1);
	value.push_back(val2);
 return value;
}

int calculate_p_value(std::vector<char*> dea){
	//calculate the player value
	std::vector<int> dea_value = calculate_hand_value(dea);
	//std::cout <<" Integer value :"<< dea_value[0] << dea_value[1] << '\n';
	int mm;
	if(dea_value[0] !=dea_value[1]){
			if(dea_value[0] > dea_value[1]){
				if(dea_value[0]<22){
					mm = dea_value[0];
				}
				else{
					mm = dea_value[1];
				}
			}
			else{
				if(dea_value[1]<22){
					mm = dea_value[1];
				}
				else{
					mm = dea_value[0];
				}
			}
	}
	else{
		mm = dea_value[0];
	}
	return mm;
}

int check_more(vector<char*> car){
	//checks if the value of handvalue is more than 21 or not. If yes, return 1 else 0
		int m = calculate_p_value(car);
		if(m > 21){
      return 1;
    }
    return 0;
}

int blackjack_(vector<char*> car){
	//checks if the value of handvalue is blackjack or not. If yes, return 1 else 0
		int m = calculate_p_value(car);
		if(m == 21){
      return 1;
    }
    return 0;
}


/*---------------------------*/

std::vector<char*> card_fir;

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
               std::cout << '\n';
               std::cout << "Your first card " << read_msg_.ca.c1_face << " of "<< read_msg_.ca.c1_suit<< std::endl;
               std::cout << "Your second card " << read_msg_.ca.c2_face << " of "<< read_msg_.ca.c2_suit<< std::endl;
               std::cout << "Dealer first card " << read_msg_.ca.d1_face << " of "<< read_msg_.ca.d1_suit<< std::endl;
               std::cout << '\n';
               std::cout << "------------------------------------------" << '\n';

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

              std::cout << "--------------------------------------" << '\n';
              std::cout << '\n';
              std::cout << "Your card value: " << read_msg_.gs.p_value << '\n';
              std::cout << "Dealer card value: " << read_msg_.gs.d_value << '\n';
              std::cout  << '\n';
   						std::cout << "--------------------------------------" << '\n';
              std::cout << '\n';
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
              exit(0);
            }
            if(blackjack_(user_cards_f)== 1){
              std::cout << "Congratulations, You have the blackJack. Press 'ENTER' to see result. " << '\n';
            }
            if (read_msg_.gs.valid && read_msg_.ca.stand != true && blackjack_(user_cards_f) == 0 ){
              std::cout << "/* -------------------------------- */" << '\n';
              std::cout << '\n';
              std::cout << "/* What you like to do? */" << '\n';
              std::cout << "Enter 'Hit' for Hit" << '\n';
              std::cout << "Enter 'Stand' for Stand or to deal." << '\n';
              std::cout << "Enter 'Split' for Split" << '\n';
              std::cout << "Enter 'Insurance' for Insurance" << '\n';
              std::cout << "Enter 'New' to start the new game. *Game should be over*" << '\n';
              std::cout << "Enter 'Exit' leave the game. *Please do Stand before Exit*" << '\n';
              std::cout << '\n';
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
              std::cout << '\n';
              std::cout << "Press 'ENTER' and 'New' to start the new game or 'Exit' to leave the game. " << '\n';
              std::cout << '\n';
            }
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
  std::cout << "Do you like to start the game? Enter ('yes') to start: ";
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
    std::cout << " ------Welcome to the CASINO ROYALE------" << '\n';
    std::cout << " Every Player Starts with 100$ credits \n ---------------------------------------------------- \n BLACKJACK WILL PLAY 3 TO 2 \n WIN PLAYS 2 TO 1 \n ---------------------------------------------------- \n\n  " << '\n';

    int result = startgame();
    if(result == 0){
      msg.ca.start_game = true;
    }

    std::cout << "Enter your name (optional): ";
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
        if(check_more(user_cards_f) == 1){
          std::cout << "*** You are busted. Card Value more than 21 ***" << '\n';
          msg.ca.hit = false;
          msg.ca.stand = true;
          msg.ca.insurance = false;
          msg.ca.split = false;
          msg.ca.bet = false;
          msg.ca.new_round = false;
          msg.gs.tip = false;
        }else{
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
        msg.gs.tip  = false;
      }
      }
      else if(strcmp(nam, "Stand")==0){
        char an[chat_message::max_body_length + 1];
        std::cout << "Are you sure you want to stand? ";
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
        std::cout << "Thank you for playing with us." << '\n';
        exit(0);
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
