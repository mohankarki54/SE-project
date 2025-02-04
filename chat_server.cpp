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
#include <thread>         // std::this_thread::sleep_for

#include "asio.hpp"
#include "chat_message2.hpp"

using asio::ip::tcp;
using namespace std;

int xxx;

int num = 0;

float id_credit = 100;

float dealer_cred = 500;

int player_turn = 1;
int co_num = 0;

int dealer_flag = 1;


int blackjack1;

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
std::vector<int> dealer_values;

//----------------------------------------------------------------------
string faces[] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};

//-------------------------------------------
class Make_Card{
	//creates the deck of the deck and suffle it
	public:
		void createDeck(){
			for(int i = 0; i<6; i++){
				for(int count = 0; count < 52; count++){
					all_card.push_back(card(&faces[count % 13][0],  &suits[count / 13][0]));
				}
			}
		}

		void suffleDeck(){
			// obtain a time-based seed:
				unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
				std::default_random_engine e(seed);
				std::shuffle(all_card.begin(), all_card.end(), e);
		}
};

int card_value(char* card){
	//calculate the individual card value..
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
	//check the card is ace or not. If yes, return 1 else 0;
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
	//Retunrs the random number generated.
	srand (time(NULL));
	int num = rand() % 311;
	return num;
}

int dealer_rand_number(){
	//Retunrs the random number generated.
	int num = rand() % 311;
	return num;
}


//Selection of Dealer cardcard
int k = random_number();
int m = dealer_rand_number();


std::vector<int> calculate_hand_value (vector<card> cards)
{
//When Ace there will be two value. returns the calculate the hand value.
	int N = cards.size();
	std::vector<int> value;
	int val1 = 0;
	int val2 = 0;
	int i;
	for(i = 0; i< N; i++){
		if(is_ace(cards[i].get_face())){
			val1 += 1;
			val2 += 11;
		}else{
			val1 += card_value(cards[i].get_face());
			val2 += card_value(cards[i].get_face());
		}
	}
	value.push_back(val1);
	value.push_back(val2);
 return value;
}

int check_blackjack(vector<card> car){
	//Returns 1 if the card is blackjack else 0;
		std::vector<int> v = calculate_hand_value(car);
		int k = v.size();
		for(int i = 0; i < k; i++ ){
			if(v[i] == 21){
				return 1;
			}
		}
		return 0;
}

int cal_value(std::vector<card> dea){
	//return the hand value.
	std::vector<int> dea_value = calculate_hand_value(dea);
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

int calculate_p_value(std::vector<card> dea){
	//return the hand value.
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

int calculate_s_value(std::vector<card> s_1, std::vector<card> s_2){
	//return the hand value.

	std::vector<int> dea_value = calculate_hand_value(s_1);
	std::vector<int> dea_value1 = calculate_hand_value(s_2);
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

	int mm1;
	if(dea_value1[0] !=dea_value1[1]){
			if(dea_value1[0] > dea_value1[1]){
				if(dea_value1[0]<22){
					mm1 = dea_value1[0];
				}
				else{
					mm1 = dea_value1[1];
				}
			}
			else{
				if(dea_value1[1]<22){
					mm1 = dea_value1[1];
				}
				else{
					mm1 = dea_value1[0];
				}
			}
	}
	else{
		mm1 = dea_value1[0];
	}

	int ans = std::max(mm, mm1);
	return ans;
}

//---------------------------------------------------------------
class chat_participant{
	//This class creates the players and its attribute.
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
  int id;
  float credit;
  string msg;
	std::vector<card> user_cards;
	std::vector<int> hand_value;
	std::vector<card> user_1;
	std::vector<card> user_2;
	char* name;
  chat_participant(){
      id = num+1;
      credit = id_credit;
  }

};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;
std::vector<chat_participant_ptr> game_user;
//----------------------------------------------------------------------

class chat_room
{
	//In this class, it take cares of the user join, leave and delivering message.
public:

  void join(chat_participant_ptr participant)
  {
	num++;
  cout<<"Player id: "<< participant->id <<" joined with credit $"<< id_credit <<endl;
	participants_.insert(participant);
	game_user.push_back(participant);

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
    cout<<"Player with id "<< participant_id << " left the game."<<endl;
		if(s >= 0 ){
			std::cout <<"Player "<< participant->id <<" wins $"<< s << '\n';
			std::cout << "------------------------------" << '\n';
		}
		else{
			int m = -1 * s;
			std::cout << "Player " <<participant->id <<" Lose $"<< m << '\n';
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
	//Server start the game session and takes care of the every functionalites of the black jack server
//Encoding and decoding of the message happens in this function. 
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
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {

					if(id == player_turn){

						if(read_msg_.gs.tip){
							read_msg_.gs.dealer_credit = dealer_cred + read_msg_.gs.tip;
							dealer_cred = read_msg_.gs.dealer_credit;
							read_msg_.gs.tip = true;
						}
						else{
						xxx = id;
						player_turn +=1;
						int kk = game_user.size();
						if(player_turn == kk+1){
							player_turn = 1;
						}
						read_msg_.gs.user_id = id;
						std::cout << "Player "<< id <<" bet $" << read_msg_.ca.bet_amo_ << std::endl;
            read_msg_.gs.valid = true;

					if(!read_msg_.gs.v_greater){
						if(read_msg_.ca.new_round){
							//std::cout << "Player name" <<" " << read_msg_.ca.name << std::endl;
							user_cards.clear();
							dealer_card.clear();

							read_msg_.ca.r_value = 1;

							int n = dealer_rand_number();
							int j = dealer_rand_number();

							strcpy(read_msg_.ca.c1_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c1_suit, all_card[n].get_suit());

							strcpy(read_msg_.ca.c2_face, all_card[j].get_face());
							strcpy(read_msg_.ca.c2_suit, all_card[j].get_suit());

							user_cards.push_back(all_card[n]);
							all_card.erase(all_card.begin()+n);

							user_cards.push_back(all_card[j]);
							all_card.erase(all_card.begin()+j);

							if(dealer_flag == 1){
								dealer_flag = 0;
								strcpy(read_msg_.ca.d1_face, all_card[k].get_face());
								strcpy(read_msg_.ca.d1_suit, all_card[k].get_suit());

								strcpy(read_msg_.ca.d2_face, all_card[m].get_face());
								strcpy(read_msg_.ca.d2_suit, all_card[m].get_suit());

								dealer_card.push_back(all_card[k]);

								all_card.erase(all_card.begin()+k);

								dealer_card.push_back(all_card[m]);
								all_card.erase(all_card.begin()+m);
							}
							else{
								strcpy(read_msg_.ca.d1_face, dealer_card[0].get_face());
								strcpy(read_msg_.ca.d1_suit, dealer_card[0].get_suit());

								strcpy(read_msg_.ca.d2_face, dealer_card[1].get_face());
								strcpy(read_msg_.ca.d2_suit, dealer_card[1].get_suit());
							}
							blackjack1 = check_blackjack(user_cards);
							read_msg_.ca.bet = true;
							std::cout << "/*---------------------------------*/" << '\n';
						}
						else if(read_msg_.ca.track_num == 0){
							read_msg_.gs.players_credit = credit;
							std::cout << "Player name" <<" " << read_msg_.ca.name << std::endl;
							int n = dealer_rand_number();
							int j = dealer_rand_number();


							strcpy(read_msg_.ca.c1_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c1_suit, all_card[n].get_suit());

							strcpy(read_msg_.ca.c2_face, all_card[j].get_face());
							strcpy(read_msg_.ca.c2_suit, all_card[j].get_suit());

							user_cards.push_back(all_card[n]);
							all_card.erase(all_card.begin()+n);

							user_cards.push_back(all_card[j]);
							all_card.erase(all_card.begin()+j);

							if(dealer_flag == 1){
								dealer_flag = 0;
							strcpy(read_msg_.ca.d1_face, all_card[k].get_face());
							strcpy(read_msg_.ca.d1_suit, all_card[k].get_suit());

							strcpy(read_msg_.ca.d2_face, all_card[m].get_face());
							strcpy(read_msg_.ca.d2_suit, all_card[m].get_suit());

							dealer_card.push_back(all_card[k]);
							all_card.erase(all_card.begin()+k);

							dealer_card.push_back(all_card[m]);
							all_card.erase(all_card.begin()+m);
						}
						else{
							strcpy(read_msg_.ca.d1_face, dealer_card[0].get_face());
							strcpy(read_msg_.ca.d1_suit, dealer_card[0].get_suit());

							strcpy(read_msg_.ca.d2_face, dealer_card[1].get_face());
							strcpy(read_msg_.ca.d2_suit, dealer_card[1].get_suit());
						}
							blackjack1 = check_blackjack(user_cards);
							std::cout << "/*---------------------------------*/" << '\n';
						}
						else if(read_msg_.ca.hit){
							if(all_card.size()==0){
								Make_Card make_card;
								make_card.createDeck();
								make_card.suffleDeck();
							}
							int k1;
							std::cout << "Player asked the new card" << '\n';
						if(read_msg_.ca.split !=true){
							int n = random_number();
							strcpy(read_msg_.ca.c1_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c1_suit, all_card[n].get_suit());
							user_cards.push_back(all_card[n]);
							all_card.erase(all_card.begin()+n);
							k1 = calculate_p_value(user_cards);
							std::cout << "----------------------------" << '\n';
						}
						else{
							std::cout << "Player have split cards." << '\n';
							int n = random_number();
							int n1 = dealer_rand_number();

							strcpy(read_msg_.ca.c1_face, all_card[n].get_face());
							strcpy(read_msg_.ca.c1_suit, all_card[n].get_suit());

							strcpy(read_msg_.ca.c2_face, all_card[n1].get_face());
							strcpy(read_msg_.ca.c2_suit, all_card[n1].get_suit());

							user_1.push_back(all_card[n]);
							all_card.erase(all_card.begin()+n);

							user_2.push_back(all_card[n1]);
							all_card.erase(all_card.begin()+n1);
							k1 = calculate_s_value(user_1, user_2);
							std::cout << "----------------------------" << '\n';
						}

						if(k1 > 21){
							read_msg_.gs.v_greater = true;
						}
					}
					else if(read_msg_.ca.stand)
						{
						/*if(dealer_flag==0){
							dealer_flag == 1;
						}*/
						if(dealer_cred <= 0){
							std::cout << '\n';
							std::cout << "**** Refilling the bank Credit ****" << '\n';
							std::cout << '\n';
							dealer_cred = 500;
							read_msg_.gs.dealer_credit = 500;
							std::this_thread::sleep_for (std::chrono::seconds(30));
						}
						hand_value = calculate_hand_value(user_cards);

						int mm = cal_value(dealer_card);
							while(mm < 17){
								int o = dealer_rand_number();
								dealer_card.push_back(all_card[o]);
								all_card.erase(all_card.begin()+o);
								int kk = cal_value(dealer_card);
								mm = kk;
							}

						int play_value;
						if(read_msg_.ca.split){
							play_value = calculate_s_value(user_1, user_2);
						}
						else{
							play_value = calculate_p_value(user_cards);
						}
						int ss = user_cards.size();
						std::cout << "---------------------------------" << '\n';
						std::cout << "Users Card" << '\n';
						for(int i = 0; i < ss; i++){
							std::cout << user_cards[i].get_face()<<" "<< user_cards[i].get_suit() << '\n';
						}
						std::cout << "---------------------------------" << '\n';
						std::cout << "Dealer Cards" << '\n';
						int ss1 = dealer_card.size();
						for(int i = 0; i < ss1; i++){
							std::cout << dealer_card[i].get_face()<<" "<< dealer_card[i].get_suit() << '\n';
						}
						std::cout << "--------------------------------------" << '\n';
						if(play_value < 22){
							if(mm < 22){
								if(play_value > mm){
									read_msg_.gs.player_win = true;
								}
								else{
									read_msg_.gs.player_win = false;
								}
							}
							else{
								read_msg_.gs.player_win = true;
							}
						}
						else if(play_value > 21){
							read_msg_.gs.player_win = false;
						}
						else{
							read_msg_.gs.draw = true;
						}
						if(read_msg_.gs.draw){
							std::cout << "It is Draw. You can start the new round with new bet amount. " << '\n';
						}
						else if(check_blackjack(dealer_card) == 1 && mm == 21){
							read_msg_.gs.players_credit = credit - read_msg_.ca.bet_amo_ + read_msg_.ca.ins_amount;
							credit = read_msg_.gs.players_credit;
							read_msg_.gs.dealer_credit = dealer_cred + read_msg_.ca.bet_amo_;
							dealer_cred = read_msg_.gs.dealer_credit;
						}
						else if(check_blackjack(user_cards) == 1 && play_value == 21){
							float win = 1.5 * read_msg_.ca.bet_amo_;
							read_msg_.gs.players_credit = credit + win - read_msg_.ca.ins_amount;
							credit = read_msg_.gs.players_credit;

							read_msg_.gs.dealer_credit = dealer_cred - win + read_msg_.ca.ins_amount;
							dealer_cred = read_msg_.gs.dealer_credit;
						}
						else if(read_msg_.gs.player_win){
							if(read_msg_.gs.blackjack == 1){
								std::cout << "---------------------------------" << '\n';
								float win = 1.5 * read_msg_.ca.bet_amo_;
								read_msg_.gs.players_credit = credit + win;
								credit = read_msg_.gs.players_credit;

								read_msg_.gs.dealer_credit = dealer_cred - win;
								dealer_cred = read_msg_.gs.dealer_credit;
							}
							else{
								std::cout << "---------------------------------" << '\n';
								read_msg_.gs.players_credit = credit + read_msg_.ca.bet_amo_;
								credit = read_msg_.gs.players_credit;
								read_msg_.gs.dealer_credit = dealer_cred - read_msg_.ca.bet_amo_;
								dealer_cred = read_msg_.gs.dealer_credit;
							}
						}
						else{
							std::cout << "---------------------------------" << '\n';
							read_msg_.gs.players_credit = credit - read_msg_.ca.bet_amo_;
							credit = read_msg_.gs.players_credit;
							read_msg_.gs.dealer_credit = dealer_cred+ read_msg_.ca.bet_amo_;
							dealer_cred = read_msg_.gs.dealer_credit;
						}

						std::cout << "Dealer hand value: "<<  mm << '\n';
						std::cout << "Players Hand valye: "<<  play_value << '\n';
						read_msg_.gs.d_value = mm;
						read_msg_.gs.p_value = play_value;
						std::cout << "Player "<< id <<" after stand have $" << read_msg_.gs.players_credit << std::endl;
						std::cout << "Current Dealer Credit $" << read_msg_.gs.dealer_credit << std::endl;
						std::cout << "-----------------------------" << '\n';

					}

					else if(read_msg_.ca.split){
							std::cout << "Player want to split." << '\n';
							if(strcmp(user_cards[0].get_face(), user_cards[1].get_face())==0){
								read_msg_.ca.split = true;
								user_1.push_back(user_cards[0]);
								user_2.push_back(user_cards[1]);
							}
							else{
								std::cout << "----No split-----" << '\n';
								read_msg_.ca.split = false;
								int n = random_number();
								strcpy(read_msg_.ca.c1_face, all_card[n].get_face());
								strcpy(read_msg_.ca.c1_suit, all_card[n].get_suit());
								user_cards.push_back(all_card[n]);
								all_card.erase(all_card.begin()+n);
								std::cout << "----------------------------" << '\n';
							}
						}
						else if(read_msg_.ca.insurance){
							std::cout << "Players have insurance." << '\n';
							if(strcmp(dealer_card[0].get_face(), "Ace") !=0){
								read_msg_.ca.insurance = false;
							}
						}

					}
					else{
						int play_value;
						if(read_msg_.ca.split){
							play_value = calculate_s_value(user_1, user_2);
						}
						else{
							play_value = calculate_p_value(user_cards);
						}
						int ss = user_cards.size();
						std::cout << "---------------------------------" << '\n';
						std::cout << "Users Card" << '\n';
						for(int i = 0; i < ss; i++){

							std::cout << user_cards[i].get_face()<<" "<< user_cards[i].get_suit() << '\n';
						}
						std::cout << "---------------------------------" << '\n';
						std::cout << "Dealer Cards" << '\n';
						int ss1 = dealer_card.size();
						for(int i = 0; i < ss1; i++){
							std::cout << dealer_card[i].get_face()<<" "<< dealer_card[i].get_suit() << '\n';
						}
						std::cout << "--------------------------------------" << '\n';

						int mm = cal_value(dealer_card);
						if(play_value < 22){
							if(mm < 22){
								if(play_value > mm){
									read_msg_.gs.player_win = true;
								}
								else{
									read_msg_.gs.player_win = false;
								}
							}
							else{
								read_msg_.gs.player_win = true;
							}
						}
						else if(play_value > 21){
							read_msg_.gs.player_win = false;
						}
						else{
							read_msg_.gs.draw = true;
						}
						if(read_msg_.gs.draw){
							std::cout << "It is Draw. You can start the new round with new bet amount. " << '\n';
						}
						else if(check_blackjack(dealer_card) == 1 && mm == 21){
							read_msg_.gs.players_credit = credit - read_msg_.ca.bet_amo_ + read_msg_.ca.ins_amount;
							credit = read_msg_.gs.players_credit;
							read_msg_.gs.dealer_credit = dealer_cred + read_msg_.ca.bet_amo_;
							dealer_cred = read_msg_.gs.dealer_credit;
						}
						else if(check_blackjack(user_cards) == 1 && play_value == 21){
							float win = 1.5 * read_msg_.ca.bet_amo_;
							read_msg_.gs.players_credit = credit + win - read_msg_.ca.ins_amount;
							credit = read_msg_.gs.players_credit;

							read_msg_.gs.dealer_credit = dealer_cred - win + read_msg_.ca.ins_amount;
							dealer_cred = read_msg_.gs.dealer_credit;
						}
						else if(read_msg_.gs.player_win){
							if(read_msg_.gs.blackjack == 1){
								std::cout << "---------------------------------" << '\n';
								float win = 1.5 * read_msg_.ca.bet_amo_;
								read_msg_.gs.players_credit = credit + win;
								credit = read_msg_.gs.players_credit;

								read_msg_.gs.dealer_credit = dealer_cred - win;
								dealer_cred = read_msg_.gs.dealer_credit;
							}
							else{
								std::cout << "---------------------------------" << '\n';
								read_msg_.gs.players_credit = credit + read_msg_.ca.bet_amo_;
								credit = read_msg_.gs.players_credit;
								read_msg_.gs.dealer_credit = dealer_cred - read_msg_.ca.bet_amo_;
								dealer_cred = read_msg_.gs.dealer_credit;
							}
						}
						else{
							std::cout << "---------------------------------" << '\n';
							read_msg_.gs.players_credit = credit - read_msg_.ca.bet_amo_;
							credit = read_msg_.gs.players_credit;
							read_msg_.gs.dealer_credit = dealer_cred+ read_msg_.ca.bet_amo_;
							dealer_cred = read_msg_.gs.dealer_credit;
						}

						std::cout << "Dealer hand value: "<<  mm << '\n';
						std::cout << "Players Hand valye: "<<  play_value << '\n';
						read_msg_.gs.d_value = mm;
						read_msg_.gs.p_value = play_value;
						std::cout << "Player "<< id <<" after stand have $" << read_msg_.gs.players_credit << std::endl;
						std::cout << "Dealer after stand have $" << read_msg_.gs.dealer_credit << std::endl;
						std::cout << "-----------------------------" << '\n';

					}
						read_msg_.encode_header();


					}
						do_read_body();
				}
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
		std::cout << "-----------------------------------" << '\n';
		std::cout << "Starting the BlackJack Game Server" << std::endl;
		std::cout << "-----------------------------------" << '\n';
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
