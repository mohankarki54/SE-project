//
// chat_server.cpp
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
#include <list>
#include <memory>
#include <set>
#include <utility>

#include <algorithm>    // std::shuffle
#include <vector>       // std::vector
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <time.h>
#include<string>

#include "asio.hpp"
#include "chat_message2.hpp"

#include <gtk/gtk.h>

using asio::ip::tcp;
using namespace std;

/* Demo to find the card value
vector<card> Car;
Car.push_back(all_card[0]);
Car.push_back(all_card[1]);
std::vector<int> v;
v = calculate_hand_value(Car);
int k = v.size();
std::cout << k << '\n';
for(int i = 0; i< k; i++){
	std::cout << v[i] << '\n';
}*/

int xxx;

int num = 0;
float id_credit = 100;
float dealer_cred = 500;

int co_num = 0;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//---------------------

card::card(char* cardFace, char* cardSuit){
		face = cardFace;
		suit = cardSuit;
}

char* card::get_face(){
		return face;
	}

char* card::get_suit(){
		return suit;
	}

//---------------------
vector<card> all_card;
std::vector<card> dealer_card;

//----------------------------------------------------------------------
string faces[] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};

//-------------------------------------------
class Make_Card{
	public:
		void createDeck(){
			for(int i = 0; i<6; i++){
				for(int count = 0; count < 52; count++){
					all_card.push_back(card(&faces[count % 13][0],  &suits[count / 13][0]));
				}
			}
		/*for(int j = 0; j< 10; j++){
				std::cout <<  all_card[j].get_face()<<" "<< all_card[j].get_suit() <<'\n';
			}
			std::cout << all_card.size() << '\n';*/
		}

		void suffleDeck(){
			// obtain a time-based seed:
				unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
				std::default_random_engine e(seed);
				std::shuffle(all_card.begin(), all_card.end(), e);
				/*for(int j = 0; j< 10; j++){
					std::cout <<  all_card[j].get_face()<<" "<< all_card[j].get_suit() <<'\n';
				}
			std::cout << all_card.size() << '\n';*/
		}
};

int card_value(char* card){
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
	int value;
	if(strcmp(card, "Ace") == 0){
		value =  1;
	}
	else{
		value = 0;
	}
	return value;
}

int random_number(){
	srand (time(NULL));
	/* generate secret number between 0 and 312: */
	int num = rand() % 311;
	return num;
}

int dealer_rand_number(){
	//srand (time(1));
	/* generate secret number between 0 and 312: */
	int num = rand() % 311;
	return num;
}

std::vector<int> calculate_hand_value (vector<card> cards)
{
//When Ace there will be two value.
	int N = cards.size();
	std::vector<int> value;
	int val1 = 0;
	int val2 = 0;
	int i;
	for(i = 0; i< N; i++){
		if(is_ace(cards[i].get_face())){
			val1 += 1;
			val2 += 11;
			value.push_back(val1);
			value.push_back(val2);
		}else{
			val1 += card_value(cards[i].get_face());
			val2 += card_value(cards[i].get_face());
			value.push_back(val1);
			value.push_back(val2);
		}

	}
 return value;
}

//---------------------------------------------------------------
class chat_participant{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
  int id;
  float credit;
  string msg;
	std::vector<card> user_cards;
	std::vector<int> hand_value;
	char* name;
  chat_participant(){
      id = num+1;
      credit = id_credit;
  }

};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:

	/*bool all_players_have_a_name()
 {
		bool retval = true;
		for (auto participant: participants_)
		{
			retval = retval &&  (participant->name > "");
		}
		return retval;
 }
*/
  void join(chat_participant_ptr participant)
  {
	num++;
  cout<<"Player id: "<<participant->id<<" Joined."<<endl;
	cout<<"Player id: "<<participant->id<<" credit: "<< id_credit <<endl;

	participants_.insert(participant);

	//Sent the message to all the participant
  for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
		num = num - 1;
  	int participant_id = participant->id;
		int s = participant->credit - 100;
    participants_.erase(participant);
    cout<<"Player with id "<< participant_id << " left the game"<<endl;
		if(s >= 0 ){
			std::cout << participant->name <<" wins $"<< s << '\n';
			std::cout << "------------------------------" << '\n';
		}
		else{
			std::cout << participant->name <<" Lose $"<< -1 * s << '\n';
			std::cout << "------------------------------" << '\n';
		}

  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_){
          if (participant->id== xxx){
            participant->deliver(msg);
        }
		}
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------


//----------------------------
class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {

  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }


private:
  void do_read_header()
  {
    //static int count=0;
	//	read_msg_.gs.dealer_credit = 500;
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            read_msg_.gs.valid = true;
						if(read_msg_.ca.track_num == 0){
							//strcpy(name, read_msg_.ca.name);
							std::cout << "Player name" <<" " << read_msg_.ca.name << std::endl;
							int n = random_number();
							strcpy(read_msg_.ca.c_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c_suit, all_card[n].get_suit());

							user_cards.push_back(all_card[n]);
							hand_value = calculate_hand_value(user_cards);

							std::cout << "/*---------------------------------*/" << '\n';
							std::cout << "Players cards " << all_card[n].get_face() << " of "<< all_card[n].get_suit() << std::endl;
						}
						else if(read_msg_.ca.hit){
							std::cout << "Player asked the new card" << '\n';
							int n = random_number();
							user_cards.push_back(all_card[n]);
							hand_value = calculate_hand_value(user_cards);
							strcpy(read_msg_.ca.c_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c_suit, all_card[n].get_suit());

					std::cout << "----------------------------" << '\n';
							if(hand_value[0] != hand_value[1]){
							std::cout << "Current hand value " << hand_value[0] << '\n';
							std::cout << "Current hand value " << hand_value[1] << '\n';
						}
						else{
							std::cout << "Current hand value " << hand_value[0] << '\n';
						}
						std::cout << "----------------------------" << '\n';
							std::cout << "Players cards " << all_card[n].get_face() << " of "<< all_card[n].get_suit() << std::endl;
						}
						else if(read_msg_.ca.stand){
							std::cout << "Players want to stand." << '\n';
							//do something when stand
							read_msg_.ca.bet = true;
						}
						else if(read_msg_.ca.split){
							std::cout << "Player want to split." << '\n';
						}
						else if(read_msg_.ca.insurance){
							std::cout << "Players like to have insurance." << '\n';
							read_msg_.ca.bet = true;

						}
						int j = dealer_rand_number();
						std::cout << "Dealers cards " << all_card[j].get_face() << " of "<< all_card[j].get_suit() << std::endl;
						std::cout << "-----------------------------------" << '\n';

						xxx = id;
						std::cout << "---------------------------------" << '\n';
						std::cout << "Player "<< id <<" bet $" << read_msg_.ca.bet_amo_ << std::endl;
						read_msg_.gs.players_credit = credit - read_msg_.ca.bet_amo_;
						credit = read_msg_.gs.players_credit;
						read_msg_.encode_header();

					/*	if(read_msg_.ca.hit){
							int n = random_number();
							user_cards.push_back(all_card[n]);
							//all_card.erase(n);
						}

						if(read_msg_.gs.dealer_points < 17){
							int n = random_number();
							user_cards.push_back(all_card[n]);
							//all_card.erase(n);
						}*/

						std::cout << "Player "<< id <<" after bet have $" << read_msg_.gs.players_credit << std::endl;
						std::cout << "-----------------------------" << '\n';
						do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
						read_msg_.gs.players_credit = credit;

						//if (self->name>"") // quick way to see if they have entered a name
            //{
							//initially read_msg_.hit
                //if (read_msg_.ca.start_game) // also need to check in order, since everyone has a turn
                //{
                   // call the dealer class with some kind of method and
                   // argument
                  // std::string m = self->name + " has asked for a hit.";
									 std::string m = " has asked for a hit.";
                   strcpy(read_msg_.body(),m.c_str());
                   read_msg_.body_length(strlen(read_msg_.body()));
                   // also set read_msg.gs.XXXto whatever needs to go to the clients
                //}
            //}
						read_msg_.encode_header();
            room_.deliver(read_msg_);
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
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
            room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

//---------------------------------------
class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
		Make_Card make_card;
		make_card.createDeck();
		make_card.suffleDeck();
		chat_message read_msg_;
		read_msg_.gs.dealer_credit = dealer_cred;
		std::cout << "Starting the BlackJack Game Server" << std::endl;
		std::cout << "Dealers credit: "<< read_msg_.gs.dealer_credit << '\n';
		read_msg_.encode_header();
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }


  return 0;
}
