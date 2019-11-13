//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

class card{
  private:
    char* face;
    char* suit;
  public:
    card(char* cardFace, char* cardSuit);
    char* get_face();
    char* get_suit();
};

// these two classes are examples of sending an
// entire structure as part of the header
class game_state{
  public:
  bool valid;
  int dealer_points;
  int player_points;
  float dealer_credit;
  float players_credit;
  bool player_win;
  bool blackjack;
  int something[3];

  // note you can't use std::string
  // or pointers
};

class client_action
{

  public:
  int track_num;
  bool join;
  bool valid;

  bool hit;
  bool stand;
  bool insurance;
  bool split;

  bool start_game;
  float bet_amo_;
  bool bet;

  char name[513];
  bool name_valid;

  char c1_face[12];
  char c1_suit[20];

  char c2_face[12];
  char c2_suit[20];

  char d1_face[12];
  char d1_suit[20];
};

class chat_message
{
public:
  enum { header_length = 11 + sizeof(client_action) + sizeof(game_state) };
  enum { max_body_length = 512 };

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, 11);
    body_length_ = std::atoi(header);
    char *p = data_ + 11; // skip the integer
    std::memcpy(&ca,p,sizeof(client_action));
    std::memcpy(&gs,p+sizeof(client_action),sizeof(game_state));

    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[11 + 1] = "";
    std::sprintf(header, "%11d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);

    char *p = data_+11; // skip over the int we just extracted
    std::memcpy(p,&ca,sizeof(client_action));
    std::memcpy(p+sizeof(client_action),&gs,sizeof(game_state));

  }

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
public:
  client_action ca;
  game_state gs;

};

#endif // CHAT_MESSAGE_HPP
