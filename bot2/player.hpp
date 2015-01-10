#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include <boost/asio.hpp>
#include <iostream>
#include <set>
using boost::asio::ip::tcp;


// number of hands to keep track in history

/*
enum LegalAction {
	BET, CALL, CHECK, DISCARD, FOLD, RAISE
};

enum PerformedAction {
	BET, CALL, CHECK, DEAL, DISCARD, FOLD, POST, RAISE, REFUND, SHOW, TIE, WIN
};

enum Street {
	FLOP, TURN, RIVER
};
*/

typedef struct {
  char rank;
  char suit;
} Card;


inline bool operator<(const Card& a, const Card& b) {
  if (a.rank < b.rank) return true;
  if (a.rank == b.rank && a.suit < b.suit) return true;
	return false;
}	

inline bool operator==(const Card& a, const Card& b) {
  if (a.rank == b.rank && a.suit == b.suit) return true;
  return false;
}	

typedef struct {
	Card cards[8]; //First 3 cards are pocket cards, last 5 cards are board cards.
} Hand;

typedef struct {
	std::string action;
	int amount; 
	std::string actor; // name of actor (for BET, CALL, CHECK, FOLD, POST)
	Card showcard1; // for SHOWdown
	Card showcard2; // for SHOWdown
  std::string street;
} PerformedAction;

typedef struct {
	std::string action;
	int min;
	int max;
} LegalAction;

inline bool operator<(const LegalAction& a, const LegalAction& b) {
  if (a.action.compare(b.action) < 0) return true;
 //  if (a.action.compare(b.action) == 0)  {
	// 	if (a.min < b.min) return true;
	// 	if (a.min == b.min) {
	// 		if (a.max < b.max) return true;
	// 	}
	// }
	return false;
}	

class Player {
    std::string player1; // name of player 1
    std::string player2;
    int start_stack;
    int big_blind;
    int num_hands;
    double start_time;
    int stack_size;
    double time_bank;
    int hand_id;
    int button;
    Card card1;
    Card card2;
    Card card3;
    Card used_cards[2];
    Card discarded;
    int my_bank;
    int opp_bank;
	Hand hands;
	std::vector<PerformedAction> action_list; // actionlist[i] is vector of Actions for handover
	int pot_size;
	std::set<LegalAction> legal_actions;
	int num_boardcards;
	double my_equity;
	double opp_equity;
  public:
    Player();
    void run(tcp::iostream &stream);
	void new_game(std::string);
	void new_hand(std::string);
	void hand_over(std::string);
	void get_action(std::string);
	
	void get_action_respond(tcp::iostream &stream);
	
	Card str_to_card(std::string);
	void calculate_equity_random_opp();

};
#endif  // __PLAYER_HPP__
