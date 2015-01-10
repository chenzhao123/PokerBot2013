#include <iostream>
#include "player.hpp"
#include <stdlib.h>
#include <cmath>
#include "pbots_calc.h"

Player::Player() {
	//create_equity250();

	opp_preflop_raise = 0;
	opp_preflop_total_raise = 0;
	opp_preflop_fold = 0;
	opp_postflop_raise = 0;	//raise and bet
	opp_postflop_total_raise = 0;
	opp_postflop_fold = 0;
	total_postflop_hands = 0;

	preflop_threshold1 = 12;
	preflop_threshold2 = 24;
	preflop_threshold3 = 40;
	postflop_threshold1 = 0.6;
	postflop_threshold2 = 0.75;

	opp_tight = 0;
}

/**
 * Simple example pokerbot, written in C++.
 *
 * This is an example of a bare bones pokerbot. It only sets up the socket
 * necessary to connect with the engine (using a Boost iostream for
 * convenience) and then always returns the same action.  It is meant as an
 * example of how a pokerbot should communicate with the engine.
 */
void Player::run(tcp::iostream &stream) {
	std::string line;
	while (std::getline(stream, line)) {
		// For now, just print out whatever date is read in.
		std::cout << line << "\n";
		std::string getaction("GETACTION");
		std::string request_keyvalue("REQUESTKEYVALUES");
		std::string newgame("NEWGAME");
		std::string keyvalue("KEYVALUE");
		std::string newhand("NEWHAND");
		std::string handover("HANDOVER");
		std::string first_word = line.substr(0, line.find_first_of(' '));
		if (newgame.compare(first_word) == 0) {
			//Call handle new game
			new_game(line.substr(line.find_first_of(' ')+1));
		} else if (getaction.compare(first_word) == 0) {
			// Respond with CHECK when playing, you'll want to change this.
			get_action(line.substr(line.find_first_of(' ')+1));
			get_action_respond(stream);

			//stream << "CHECK\n";
		} else if (request_keyvalue.compare(first_word) == 0) {
		
			// tests
			//std::cout << player1 << " " << player2 << " " << num_hands << " " << my_bank << "\n";
			//for (int i=0; i<10; i++)
			//	std::cout << action_list[i][0].action << ":" 
			//						<< action_list[i][0].actor << "\n";
			// end tests
			request_keyvalue_action(stream);
			// FINISh indicates no more keyvalue pairs to store.
			stream << "FINISH\n";
		} else if (keyvalue.compare(first_word) == 0){
			//Call handle keyvalue
			key_value(line.substr(line.find_first_of(' ')+1));
		} else if (newhand.compare(first_word) == 0){
			//Call handle newhand
			new_hand(line.substr(line.find_first_of(' ')+1));
			/*
			if (hand_id == 900) {
				std::cout << "preflop raise: "  << opp_preflop_raise << std::endl;
				std::cout << "postflop fold: "  << opp_postflop_fold << std::endl;
				std::cout << "opp preflop raise: " << opp_preflop_raise << std::endl;
				std::cout << "opp preflop fold: " << opp_preflop_fold << std::endl;
				std::cout << "opp postflop raise: " << opp_postflop_raise << std::endl;
				std::cout << "opp preflop raise amt: " << opp_preflop_total_raise << std::endl;
				std::cout << "opp postflop raise amt: " << opp_postflop_total_raise << std::endl;
				std::cout << "opp postflop fold: " << opp_postflop_fold << std::endl;
				std::cout << "total flop hands: " << total_postflop_hands << std::endl;
			}
			*/
		} else if (handover.compare(first_word) == 0){
			//Call handle handover
			hand_over(line.substr(line.find_first_of(' ')+1));
		}

	}

	std::cout << "Gameover, engine disconnected.\n";
}

void Player::new_game(std::string line){
	int space_index = line.find_first_of(' ');
	player1 = line.substr(0, space_index);
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	player2 = line.substr(0, space_index);
	line = line.substr(space_index+1);
	
	space_index = line.find_first_of(' ');
	start_stack = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	big_blind = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	num_hands = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	start_time = atof(line.c_str());

}

void Player::new_hand(std::string line){
	int space_index = line.find_first_of(' ');
	hand_id = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	button = !(line.substr(0, space_index).compare("true"));	
	// button=1 if string==true, else button=0
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	std::string c1 = line.substr(0, space_index);
	card1.rank = c1[0];
	card1.suit = c1[1];
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	std::string c2 = line.substr(0, space_index);
	card2.rank = c2[0];
	card2.suit = c2[1];
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	std::string c3 = line.substr(0, space_index);
	card3.rank = c3[0];
	card3.suit = c3[1];
	line = line.substr(space_index+1);

	hands.cards[0] = card1;
	hands.cards[1] = card2;
	hands.cards[2] = card3;

	space_index = line.find_first_of(' ');
  my_bank = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	opp_bank = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	time_bank = atof(line.c_str());

}

void Player::get_action(std::string line){

  // clear legal action list

  legal_actions.clear();

	int space_index, colon_index;

	space_index = line.find_first_of(' ');
	pot_size = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	num_boardcards = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	for(int i=0; i<num_boardcards; i++) {
		space_index = line.find_first_of(' ');
		std::string c = line.substr(0, space_index);
		Card card;
		card.rank = c[0];
		card.suit = c[1];
		hands.cards[i+3] = card; 
		line = line.substr(space_index+1);
	}

	space_index = line.find_first_of(' ');
	int num_lastactions = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	for(int i=0; i<num_lastactions; i++) {
		space_index = line.find_first_of(' ');
		std::string actstr = line.substr(0, space_index); // string of one action
		PerformedAction act;
		// find : separator
		colon_index = actstr.find_first_of(':');
		act.action = actstr.substr(0, colon_index);
		if (act.action.compare("BET") == 0 || act.action.compare("POST") == 0 || 
				act.action.compare("RAISE") == 0 || act.action.compare("REFUND") == 0 || 
				act.action.compare("TIE") == 0 || act.action.compare("WIN") == 0){
				actstr = actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.amount = atoi(actstr.substr(0, colon_index).c_str());
			act.actor = actstr.substr(colon_index+1);
		} else if (act.action.compare("CALL") == 0 || act.action.compare("CHECK") == 0 || 
				act.action.compare("FOLD") == 0){
			act.actor = actstr.substr(colon_index+1);
		} else if (act.action.compare("DEAL") == 0){
      act.street = actstr.substr(colon_index+1);
		} else if (act.action.compare("SHOW") == 0){
			actstr= actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.showcard1 = str_to_card(actstr.substr(0, colon_index));
			actstr = actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.showcard2 = str_to_card(actstr.substr(0, colon_index));
			act.actor = actstr.substr(colon_index+1);
    }

	  action_list.push_back(act);	// add act to action_list
	  line = line.substr(space_index+1);
	}

  space_index = line.find_first_of(' ');
  int num_legalactions = atoi(line.substr(0, space_index).c_str());
  line = line.substr(space_index+1);

  for(int i=0; i<num_legalactions; i++) {
    space_index = line.find_first_of(' ');
		std::string actstr = line.substr(0, space_index);
	  LegalAction act;
	  // find : separator
	  colon_index = actstr.find_first_of(':');
	  act.action = actstr.substr(0, colon_index);
	  if (act.action.compare("BET") == 0 || act.action.compare("RAISE") == 0){
      actstr = actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.min = atoi(actstr.substr(0, colon_index).c_str());
			act.max = atoi(actstr.substr(colon_index+1).c_str());
		} 

	  legal_actions.insert(act); // add act to legal_actions
	  line = line.substr(space_index+1);	
	}	
  
	time_bank = atof(line.c_str());
	

} // end get_action(std::string line)

void Player::hand_over(std::string line){
  	
  	int space_index, colon_index;
	 
	space_index	= line.find_first_of(' ');
	my_bank = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	opp_bank = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	space_index = line.find_first_of(' ');
	num_boardcards = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

  for(int i=0; i<num_boardcards; i++) {
		space_index = line.find_first_of(' ');
	  std::string c = line.substr(0, space_index);
		Card card;
	  card.rank = c[0];
	  card.suit = c[1];
		hands.cards[i+3] = card; // keep track of last 1000 hands
	  line = line.substr(space_index+1);
	}

	space_index = line.find_first_of(' ');
	int num_lastactions = atoi(line.substr(0, space_index).c_str());
	line = line.substr(space_index+1);

	for(int i=0; i<num_lastactions; i++) {
		space_index = line.find_first_of(' ');
		std::string actstr = line.substr(0, space_index); // string of one action
		PerformedAction act;
		// find : separator
		colon_index = actstr.find_first_of(':');
    act.action = actstr.substr(0, colon_index);
	  if (act.action.compare("BET") == 0 || act.action.compare("POST") == 0 || 
				act.action.compare("RAISE") == 0 || act.action.compare("REFUND") == 0 || 
				act.action.compare("TIE") == 0 || act.action.compare("WIN") == 0){
      actstr = actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.amount = atoi(actstr.substr(0, colon_index).c_str());
		  act.actor = actstr.substr(colon_index+1);
		} else if (act.action.compare("CALL") == 0 || act.action.compare("CHECK") == 0 || 
				act.action.compare("FOLD") == 0){
      act.actor = actstr.substr(colon_index+1);
		} else if (act.action.compare("DEAL") == 0){
      act.street = actstr.substr(colon_index+1);
		} else if (act.action.compare("SHOW") == 0){
			actstr= actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.showcard1 = str_to_card(actstr.substr(0, colon_index));
			actstr = actstr.substr(colon_index+1);
			colon_index = actstr.find_first_of(':');
			act.showcard2 = str_to_card(actstr.substr(0, colon_index));
			act.actor = actstr.substr(colon_index+1);
    }

	  action_list.push_back(act);	// add act to action_list
	  line = line.substr(space_index+1);
	}

  time_bank = atof(line.c_str());
  calculate_stats();
  action_list.clear();
  if (hand_id%200 == 0){
  	classify_opp();
  	opp_preflop_raise = 0;
	opp_preflop_total_raise = 0;
	opp_preflop_fold = 0;
	opp_postflop_raise = 0;	//raise and bet
	opp_postflop_total_raise = 0;
	opp_postflop_fold = 0;
	total_postflop_hands = 0;
  }
}
void Player::key_value(std::string line){
	
	int space_index = line.find_first_of(' ');
	std::string opponent = line.substr(0, space_index);
	line = line.substr(space_index+1);

	if (opponent.compare(player2) == 0){
		//std::cout << "Playing " << opponent << " again" << "\n";
		space_index = line.find_first_of(' ');
		int player2_lastbank = atoi(line.substr(0, space_index).c_str());
		line = line.substr(space_index+1);

		space_index = line.find_first_of(' ');
		int last_numhands = atoi(line.substr(0, space_index).c_str());
		line = line.substr(space_index+1);

		space_index = line.find_first_of(' ');
		int player2_tight = atoi(line.c_str());

		if (player2_lastbank > (2*last_numhands) && player2_tight){
			opp_tight = 1;
			preflop_threshold1 = 12;
			preflop_threshold2 = 30;
			preflop_threshold3 = 44;
			postflop_threshold1 = 0.65;
			postflop_threshold2 = 0.80;
		}
	}
}

void Player::request_keyvalue_action(tcp::iostream &stream){
	stream << "PUT " << player2 << " " << opp_bank << " " << num_hands << " " << opp_tight << "\n";
}

void Player::get_action_respond(tcp::iostream &stream){

  // for testing:
  // hands[hand_id%NUM_HANDS-1].cards[0] = {'A','d'};
  // hands[hand_id%NUM_HANDS-1].cards[1] = {'Q','h'};
  // hands[hand_id%NUM_HANDS-1].cards[2] = {'J','d'};

  // card1 = hands[hand_id%NUM_HANDS-1].cards[0]; 
  // card2 = hands[hand_id%NUM_HANDS-1].cards[1]; 
  // card3 = hands[hand_id%NUM_HANDS-1].cards[2]; 

  if (num_boardcards == 0){         // Pre-flop    
		get_preflop_action(stream);			

	} else if (num_boardcards == 3) { // Flop
    get_postflop_action(stream);

	} else if (num_boardcards == 4) { // Turn
    get_postflop_action(stream); 

	} else if (num_boardcards == 5) { // River
    get_postflop_action(stream); 

	}
}

void Player::get_preflop_action(tcp::iostream &stream){

	
	int bet = 4 * compute_betsize_model1(card1, card2, card3);

	if (button) {  // Dealer, Small Blind, Act first
		PerformedAction opp_action = *(action_list.end()-1);
		if ((opp_action.action).compare("POST") == 0) {
			if (bet < preflop_threshold1) stream << "FOLD\n";
			else if (bet < preflop_threshold2) stream << "CALL\n";
			else if (bet < preflop_threshold3) stream << "RAISE:" << bet/2 << "\n"; 
			else stream << "RAISE:" << bet << "\n";
		} else if ((opp_action.action).compare("RAISE") == 0){
			int raise_amount = opp_action.amount;
			if (bet < preflop_threshold1) stream << "FOLD\n";
			else if (bet < preflop_threshold2) {
				if (raise_amount <= bet) stream << "CALL\n";
				else stream << "FOLD\n";
			} else if (bet < preflop_threshold3) {
				LegalAction raise_act;
        		raise_act.action = "RAISE";
  				if (legal_actions.find(raise_act) != legal_actions.end()){
		    		LegalAction act = *(legal_actions.find(raise_act));
		    		if ((bet/2) <= act.max && (bet/2) >= act.min) stream << "RAISE:" << bet/2 << "\n";
		    		else if (raise_amount <= 1.5*bet) stream << "CALL\n";
		    		else stream << "FOLD\n";
		    	} else stream << "FOLD\n";
			} else {
        		LegalAction raise_act;
        		raise_act.action = "RAISE";
  				if (legal_actions.find(raise_act) != legal_actions.end()){
		    		LegalAction act = *(legal_actions.find(raise_act));
		    		if (bet <= act.max && bet >= act.min) stream << "RAISE:" << bet << "\n";
		    		else stream << "CALL\n";
		    	} else stream << "CALL\n";
			}
		}
	} else {  // Big Blind, Act second
		PerformedAction opp_action = *(action_list.end()-1);
		if ((opp_action.action).compare("CALL") == 0){
    		if (bet < preflop_threshold2) stream << "CHECK\n";
    		else if (bet < preflop_threshold3) stream << "RAISE:" << bet/2 << "\n"; 
			else stream << "RAISE:" << bet << "\n";
		} else if ((opp_action.action).compare("RAISE") == 0) {
    		int raise_amount = opp_action.amount;
			if (bet < preflop_threshold1) stream << "CHECK\n"; //Will auto fold if check is illegal
			else if (bet < preflop_threshold2) {
      			if (raise_amount <= bet) stream << "CALL\n";
				else stream << "CHECK\n"; //Will auto fold if check is illegal
			} else if (bet < preflop_threshold3) {
				LegalAction raise_act;
        		raise_act.action = "RAISE";
  				if (legal_actions.find(raise_act) != legal_actions.end()){
		    		LegalAction act = *(legal_actions.find(raise_act));
		    		if ((bet/2) <= act.max && (bet/2) >= act.min) stream << "RAISE:" << bet/2 << "\n";
		    		else if (raise_amount <= 1.5*bet) stream << "CALL\n";
		    		else stream << "CHECK\n";
		    	} else stream << "CHECK\n"; //Will auto fold if check is illegal
			} else {
				LegalAction raise_act;
        		raise_act.action = "RAISE";
  				if (legal_actions.find(raise_act) != legal_actions.end()){
		    		LegalAction act = *(legal_actions.find(raise_act));
		    		if (bet <= act.max && bet >= act.min) stream << "RAISE:" << bet << "\n";
		    		else stream << "CALL\n";
		    	} else stream << "CALL\n";
			} 
		}
	}
	
	/*
	calculate_equity_random_opp();
	PerformedAction opp_action = *(action_list.end()-1);
	if (my_equity > 0.6) {
		LegalAction raise_act;
		raise_act.action = "RAISE";
		if (legal_actions.find(raise_act) != legal_actions.end()){
    		LegalAction act = *(legal_actions.find(raise_act));
    		int amt = 40 * my_equity + ((rand() % 10) - 5);
    		if (amt > act.max) amt = act.max;
    		else if (amt < act.min) amt = act.min;
    		stream << "RAISE:" << amt << "\n";
    	} else stream << "CALL\n";
	} else if (best_equity > 0.6 || worst_equity > 0.52) {
		LegalAction raise_act;
		raise_act.action = "RAISE";
		if (legal_actions.find(raise_act) != legal_actions.end()){
    		LegalAction act = *(legal_actions.find(raise_act));
    		int amt = 20 * best_equity + ((rand() % 5) - 2);
    		if (amt <= act.max && amt >= act.min){
    			stream << "RAISE:" << amt << "\n";
    		} else if ((opp_action.action).compare("RAISE") == 0){
    			int raise_amount = opp_action.amount;
    			if (raise_amount < (200*best_equity - 80)){
    				stream << "CALL\n";
    			} else stream << "CHECK\n"; // Will auto fold 
			} else stream << "CHECK\n"; // Will auto fold if check is illegal.
    	} else stream << "CHECK\n"; // Will auto fold if check is illegal.
	} else stream << "CHECK\n";  // Will auto fold if check is illegal	
	*/
	
}


/////////////////////////////////////////////////////////////////////////////////

void Player::get_postflop_action(tcp::iostream &stream){

   // hands[hand_id%NUM_HANDS-1].cards[3] = {'K','h'};
   // hands[hand_id%NUM_HANDS-1].cards[4] = {'A','h'};
   // hands[hand_id%NUM_HANDS-1].cards[5] = {'7','h'};
  

  PerformedAction last_action = *(action_list.end()-1);
  calculate_equity_random_opp();

  LegalAction discard_act;
  discard_act.action = "DISCARD";
  if (legal_actions.find(discard_act) != legal_actions.end()){
    stream << "DISCARD:" << discarded.rank << discarded.suit << "\n";
    return;
  }

  //int constant = 2;  // maybe change to 1.5?
  //int bet = my_equity * pot_size * constant;
  double bet;
  double x = my_equity;
  double x2 = x*x;
  double x3 = x*x2;
  double temp1 = x2*100;
  double temp2 = x*10;
  double temp3 = x3*300;
  //std::cout << temp1 << " " << temp2 << " " << temp3 << std::endl;
  if (num_boardcards == 3) { 
  	//bet = my_equity * 60; 
  	bet = temp1 - temp2 - 10; 
  } else if (num_boardcards == 4) {
  	//bet = my_equity * 120;
  	bet = 2*temp1 + temp2 - 40;
  } else {
  	//bet = my_equity * 240;
  	bet = temp3 - 20;
  }

  int tolerance = my_equity * pot_size;
  int opp_raise = 0;
  if ((last_action.action).compare("BET") == 0) {
    opp_raise = last_action.amount;
    tolerance = my_equity * (pot_size - opp_raise);
  } else if ((last_action.action).compare("RAISE") == 0) {
    opp_raise = last_action.amount;
    tolerance = my_equity * (pot_size - opp_raise + bet); //Estimates actual pot size
  }
  

  int bet_final = bet;

  int max_amt = 0 ;
  int min_amt = 0 ;
  int canRaiseBet = 0;
  LegalAction bet_act;
  LegalAction raise_act;
  bet_act.action = "BET";
  raise_act.action = "RAISE";
  if (legal_actions.find(bet_act) != legal_actions.end()){
    LegalAction act = *(legal_actions.find(bet_act));
    max_amt = act.max; 
    min_amt = act.min;
    canRaiseBet = 1;
  }
  if (legal_actions.find(raise_act) != legal_actions.end()){
    LegalAction act = *(legal_actions.find(raise_act));
    max_amt = act.max; 
    min_amt = act.min;
    canRaiseBet = 1;
  }
  
  if (canRaiseBet) {  //Raise or Bet is a legal action.
    if (bet_final > max_amt) {
      bet_final = max_amt;
    }
    if (bet_final < min_amt) {
      bet_final = min_amt;
    }
  }

  if (my_equity < postflop_threshold1){						///// low equity 
    if (button){	
      if ((last_action.action).compare("CHECK")==0) {
        stream << "CHECK\n";
      } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        /*
        if (opp_tight && raise_amount > 20) {
        	stream << "FOLD\n";
        } else if (raise_amount < (bet/constant) || raise_amount <= 20 ){  // second condition new
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
        */
        if (raise_amount < tolerance){  
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
      }
    } else {	
      if ((last_action.action).compare("DEAL") == 0){
        stream << "CHECK\n";
      } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        /*
        if (opp_tight && raise_amount > 20) {
        	stream << "FOLD\n";
        } else if (raise_amount < (bet/constant) || raise_amount <= 20 ){  // second condition new
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
        */
        if (raise_amount < tolerance){  
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
      }
    }
  } else if (my_equity < postflop_threshold2) { 			///// medium equity
    if (button){
      if ((last_action.action).compare("CHECK")==0) {
        stream << "BET:" << bet_final << "\n" ;
      } else if ((last_action.action).compare("BET") == 0||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        if (bet > 2*raise_amount) {
          if (canRaiseBet) stream << "RAISE:" << bet_final << "\n";
          else stream << "CALL\n";
        } else if (raise_amount < tolerance || raise_amount < bet){
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
      }
    } else {
      if ((last_action.action).compare("DEAL") == 0){
        stream << "BET:" << bet_final << "\n" ;
      } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        if (bet > 2*raise_amount) {
          if (canRaiseBet) stream << "RAISE:" << bet_final << "\n";
          else stream << "CALL\n";
        } else if (raise_amount < tolerance || raise_amount < bet){
          stream << "CALL\n";
        } else {
          stream << "FOLD\n";
        }
      }
    }
  } else {													///// high equity

    if (button){
      if ((last_action.action).compare("CHECK")==0) {
        stream << "BET:" << bet_final << "\n" ;
      } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        if (bet > 2*raise_amount && canRaiseBet) {
          stream << "RAISE:" << bet_final << "\n";
        } else {
          stream << "CALL\n";
        } 
      }  
    } else {
      if ((last_action.action).compare("DEAL")==0) {
        stream << "BET:" << bet_final << "\n" ;
      } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
        int raise_amount = last_action.amount;
        if (bet > 2*raise_amount && canRaiseBet) {
          stream << "RAISE:" << bet_final << "\n";
        } else {
          stream << "CALL\n";
        } 
      } 
    }
  }
}
///////////////////////////////////////////////////////////////////////////


void Player::get_turn_action(tcp::iostream &stream){
  
  PerformedAction last_action = *(action_list.end()-1);

  if (button) { // go last, see last action
    if ((last_action.action).compare("CHECK")==0) {
      // TODO
      // compute bet amount
      // stream CHECK or BET:bet

      stream << "CHECK\n";  // for now

    } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
      int raise_amount = last_action.amount;
      // TODO
      // compute bet amount
      // stream FOLD, CALL, or RAISE:amount

      stream << "CALL\n"; // for now
    } 

  } else {
    //go first if last action is "DEAL"
    //check last_action (for CHECK and BET and RAISE)
    if ((last_action.action).compare("DEAL") == 0) { // our first action after deal
      // TODO
      // compute bet amount 
      // stream CHECK or BET:bet
      stream << "CHECK\n";   // for now

    } else if ((last_action.action).compare("BET") == 0 || 
              (last_action.action).compare("RAISE") == 0) {
      int raise_amount = last_action.amount;
      // TODO
      // compute bet amount
      // stream FOLD, CALL, or RAISE:amount
      stream << "CALL\n";    // for now
    }
  }
}


void Player::get_river_action(tcp::iostream &stream){
  
  PerformedAction last_action = *(action_list.end()-1);

  if (button) { // go last, see last action
    if ((last_action.action).compare("CHECK")==0) {
      // TODO
      // compute bet amount
      // stream CHECK or BET:bet

      stream << "CHECK\n";  // for now

    } else if ((last_action.action).compare("BET") == 0 ||
              (last_action.action).compare("RAISE") == 0) {
      int raise_amount = last_action.amount;
      // TODO
      // compute bet amount
      // stream FOLD, CALL, or RAISE:amount

      stream << "CALL\n"; // for now
    } 

  } else {
    //go first if last action is "DEAL"
    //check last_action (for CHECK and BET and RAISE)
    if ((last_action.action).compare("DEAL") == 0) { // our first action after deal
      // TODO
      // compute bet amount 
      // stream CHECK or BET:bet
      stream << "CHECK\n";   // for now

    } else if ((last_action.action).compare("BET") == 0 || 
              (last_action.action).compare("RAISE") == 0) {
      int raise_amount = last_action.amount;
      // TODO
      // compute bet amount
      // stream FOLD, CALL, or RAISE:amount
      stream << "CALL\n";    // for now
    }
  }
}


void Player::calculate_equity_random_opp() {
  
  //std::cout << "========= got here in calculate_equity_random_opp" << std::endl;
  Card c;
  char board_cards [num_boardcards * 2+1]; 
  for (int i = 0; i < num_boardcards; i++){
    c = hands.cards[i+3];
    board_cards[i*2] = c.rank;
    board_cards[i*2 + 1] = c.suit; 
  }
  board_cards[num_boardcards*2] = '\0';
  char* board = board_cards;

  if (num_boardcards == 0) { //Pre-flop
  	char hand1[] = {card1.rank, card1.suit, card2.rank, card2.suit, ':', 'x', 'x','\0'};
    char hand2[] = {card1.rank, card1.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    char hand3[] = {card2.rank, card2.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    
    char* myhand = hand1;
    char dead_card[] = {card3.rank, card3.suit,'\0'};
    char* dead = dead_card;
    Results* res1 = alloc_results();

    calc(myhand, (char*)"", dead, NUM_TRIALS, res1);


    myhand = hand2;
    dead_card[0] = card2.rank;
    dead_card[1] = card2.suit;
    Results* res2 = alloc_results();

    calc(myhand, (char*)"", dead, NUM_TRIALS, res2);

    myhand = hand3;
    dead_card[0] = card1.rank;
    dead_card[1] = card1.suit;
    Results* res3 = alloc_results();

    calc(myhand, (char*)"", dead, NUM_TRIALS, res3);

    double eq1 = *((res1->ev)+0);
    double eq2 = *((res2->ev)+0);
    double eq3 = *((res3->ev)+0);

    if (eq1 >= eq2 && eq1 >= eq3){
    	best_equity = eq1;
    } else if (eq2 >= eq1 && eq2 >= eq3){
    	best_equity = eq2;
    } else {
    	best_equity = eq3;
    }

    if (eq1 <= eq2 && eq1 <= eq3){
    	worst_equity = eq1;
    } else if (eq2 <= eq1 && eq2 <= eq3){
    	worst_equity = eq2;
    } else {
    	worst_equity = eq3;
    }
    free_results(res1);
    free_results(res2);
    free_results(res3);

    char hand[] = {card1.rank, card1.suit, card2.rank, card2.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    myhand = hand;
    Results* res = alloc_results();
    calc(myhand, (char*)"", (char*)"", NUM_TRIALS, res);
    my_equity = *((res->ev)+0);
    opp_equity = *((res->ev)+1);
    free_results(res);

  } else if (num_boardcards == 3){ //Flop
    char hand1[] = {card1.rank, card1.suit, card2.rank, card2.suit, ':', 'x', 'x','\0'};
    char hand2[] = {card1.rank, card1.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    char hand3[] = {card2.rank, card2.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};

    //std::cout << card1.rank << card1.suit << card2.rank << card2.suit << card3.rank << card3.suit << std::endl;
    
    char* myhand = hand1;
    char dead_card[] = {card3.rank, card3.suit,'\0'};
    char* dead = dead_card;
    Results* res1 = alloc_results();

    calc(myhand, board, dead, NUM_TRIALS, res1);


    myhand = hand2;
    dead_card[0] = card2.rank;
    dead_card[1] = card2.suit;
    Results* res2 = alloc_results();

    calc(myhand, board, dead, NUM_TRIALS, res2);

    myhand = hand3;
    dead_card[0] = card1.rank;
    dead_card[1] = card1.suit;
    Results* res3 = alloc_results();

    calc(myhand, board, dead, NUM_TRIALS, res3);

    double eq1 = *((res1->ev)+0);
    double eq2 = *((res2->ev)+0);
    double eq3 = *((res3->ev)+0);

    if (eq1 >= eq2 && eq1 >= eq3) {
      used_cards[0] = card1;
      used_cards[1] = card2;
      discarded = card3;        
      my_equity = eq1;
      opp_equity = 1-my_equity;
    } else if (eq2 >= eq1 && eq2 >= eq3) {
      used_cards[0] = card1;
      used_cards[1] = card3;
      discarded = card2;
      my_equity = eq2;
      opp_equity = 1 - my_equity;
    } else {
      used_cards[0] = card2;
      used_cards[1] = card3;
      discarded = card1;
      my_equity = eq3;
      opp_equity = 1-my_equity;
    }

    free_results(res1);
    free_results(res2);
    free_results(res3);

  } else { //Turn or River
    char hand[] = {used_cards[0].rank, used_cards[0].suit, used_cards[1].rank, used_cards[1].suit, ':', 'x', 'x','\0'};
    char dead_card[] = {discarded.rank, discarded.suit,'\0'};
    char* myhand = hand;
    char* dead = dead_card;
    Results* res = alloc_results();
    calc(myhand, board, dead, NUM_TRIALS, res);
    my_equity = *((res->ev)+0);
    opp_equity = *((res->ev)+1);
  	free_results(res);
  }
}

void Player::classify_opp(){

	double preflop_raise = ((double) opp_preflop_raise)/ 200;
	double preflop_fold = ((double) opp_preflop_fold)/ 200;
	double avg_preflop_raise_amt = ((double) opp_preflop_total_raise) / opp_preflop_raise;

	double postflop_raise = ((double) opp_postflop_raise)/ total_postflop_hands;
	double postflop_fold = ((double) opp_postflop_fold)/ total_postflop_hands;
	double avg_postflop_raise_amt = ((double) opp_postflop_total_raise) / opp_postflop_raise;

	double total_fold = ((double) opp_preflop_fold + opp_postflop_fold)/ 200;

	if (preflop_raise < 0.5 || postflop_raise < 0.3) { //Passive player

	} 

/*
	std::cout << "opp preflop raise: " << opp_preflop_raise << std::endl;
	std::cout << "opp preflop fold: " << opp_preflop_fold << std::endl;
	std::cout << "opp postflop raise: " << opp_postflop_raise << std::endl;
	std::cout << "opp preflop raise amt: " << opp_preflop_total_raise << std::endl;
	std::cout << "opp postflop raise amt: " << opp_postflop_total_raise << std::endl;
	std::cout << "opp postflop fold: " << opp_postflop_fold << std::endl;
	std::cout << "total postflop hands: " << total_postflop_hands << std::endl;
*/
	
	if (preflop_fold > 0.5 || postflop_fold > 0.7 || total_fold > 0.5) { //Tight player

		//std::cout << "tight player\n";

		opp_tight = 1;
		preflop_threshold1 = 12;
		preflop_threshold2 = 30;
		preflop_threshold3 = 44;
		postflop_threshold1 = 0.65;
		postflop_threshold2 = 0.8;
	} else {

		//std::cout << "not tight anymore\n";

		opp_tight = 0;
		preflop_threshold1 = 12;
		preflop_threshold2 = 24;
		preflop_threshold3 = 40;
		postflop_threshold1 = 0.6;
		postflop_threshold2 = 0.75;
	}
}

void Player::calculate_stats(){
	bool opp_raised_preflop = false;
	int opp_preflop_last_raise_amt = 0;
	bool opp_raised_flop = false;
	bool opp_raised_turn = false;
	bool opp_raised_river = false;
	int opp_flop_last_raise_amt = 0;
	int opp_turn_last_raise_amt = 0;
	int opp_river_last_raise_amt = 0;
	std::vector<PerformedAction>::iterator next_it = action_list.end();

	for (std::vector<PerformedAction>::iterator it = action_list.begin(); it != action_list.end(); ++it){
		PerformedAction act = *it;

		if (act.action.compare("DEAL") == 0){
			if (act.street.compare("FLOP") == 0) {
				next_it = it;
				total_postflop_hands++;
			}
			break;
		} else if (act.action.compare("RAISE") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_preflop)){
					opp_raised_preflop = true;
					opp_preflop_raise++;

					opp_preflop_last_raise_amt = raise_amount;
					opp_preflop_total_raise += opp_preflop_last_raise_amt;	
				} else {
					opp_preflop_total_raise -= opp_preflop_last_raise_amt;
					opp_preflop_last_raise_amt = raise_amount;
					opp_preflop_total_raise += opp_preflop_last_raise_amt;
				}
			}
		} else if (act.action.compare("FOLD") == 0){
			if (act.actor.compare(player2) == 0) opp_preflop_fold++;
		} 
	}

	for (std::vector<PerformedAction>::iterator it = next_it; it != action_list.end(); ++it){
		PerformedAction act = *it;

		if (act.action.compare("DEAL") == 0){
			if (act.street.compare("TURN") == 0) {
				next_it = it;
				total_postflop_hands++;
			}
			break;
		} else if (act.action.compare("RAISE") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_flop)){
					opp_raised_flop = true;
					opp_postflop_raise++;
					opp_flop_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_flop_last_raise_amt;	
				} else {
					opp_postflop_total_raise -= opp_flop_last_raise_amt;
					opp_flop_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_flop_last_raise_amt;
				}
			}
		} else if (act.action.compare("BET") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_flop)){
					opp_raised_flop = true;
					opp_postflop_raise++;
					opp_flop_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_flop_last_raise_amt;	
				} 
			}	
		} else if (act.action.compare("FOLD") == 0){
			if (act.actor.compare(player2) == 0) opp_postflop_fold++;
		} 
	}

	for (std::vector<PerformedAction>::iterator it = next_it; it != action_list.end(); ++it){
		PerformedAction act = *it;

		if (act.action.compare("DEAL") == 0){
			if (act.street.compare("RIVER") == 0) {
				next_it = it;
				total_postflop_hands++;
			}
			break;
		} else if (act.action.compare("RAISE") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_turn)){
					opp_raised_turn = true;
					opp_postflop_raise++;
					opp_turn_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_turn_last_raise_amt;	
				} else {
					opp_postflop_total_raise -= opp_turn_last_raise_amt;
					opp_turn_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_turn_last_raise_amt;
				}
			}
		} else if (act.action.compare("BET") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_flop)){
					opp_raised_flop = true;
					opp_postflop_raise++;
					opp_flop_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_flop_last_raise_amt;	
				} 
			}	
		} else if (act.action.compare("FOLD") == 0){
			if (act.actor.compare(player2) == 0) opp_postflop_fold++;
		} 
	}

	for (std::vector<PerformedAction>::iterator it = next_it; it != action_list.end(); ++it){
		PerformedAction act = *it;

		if (act.action.compare("RAISE") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_river)){
					opp_raised_river = true;
					opp_postflop_raise++;
					opp_river_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_river_last_raise_amt;	
				} else {
					opp_postflop_total_raise -= opp_river_last_raise_amt;
					opp_river_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_river_last_raise_amt;
				}
			}
		} else if (act.action.compare("BET") == 0){
			if (act.actor.compare(player2) == 0){
				int raise_amount = act.amount;
				if (!(opp_raised_flop)){
					opp_raised_flop = true;
					opp_postflop_raise++;
					opp_flop_last_raise_amt = raise_amount;
					opp_postflop_total_raise += opp_flop_last_raise_amt;	
				} 
			}	
		} else if (act.action.compare("FOLD") == 0){
			if (act.actor.compare(player2) == 0) opp_postflop_fold++;
		} 
	}

}


/*
input: string of 2 characters: rank, suit
returns: Card instance 
*/
Card Player::str_to_card(std::string str) {
	Card c;
	c.rank = str[0];
	c.suit = str[1];
	return c;
}


int Player::rank_to_int(char r) {
	if (r >= '2' && r <= '9') return r - '0';
	else if (r=='T') return 10;
	else if (r=='J') return 11;
	else if (r=='Q') return 12;
	else if (r=='K') return 13;
	else if (r=='A') return 14;
	else return 0;
}

int Player::compute_betsize_model1(Card c1, Card c2, Card c3) {

	int r1, r2, r3;
	r1 = rank_to_int(c1.rank);
	r2 = rank_to_int(c2.rank);
	r3 = rank_to_int(c3.rank);
	
	char s1, s2, s3;
	s1 = c1.suit;
	s2 = c2.suit;
	s3 = c3.suit;

	int sum = 2*(r1 + r2 + r3);
	if (r1 == r2 && r1 == r3) return sum/6 - 2;           		 // 3 of a kind
	else if (r1 == r2 || r1 == r3 || r2 == r3) sum+=13;  // 2 of a kind

	if (s1 == s2 && s1 == s3) sum+=4;                    // 3 suited
	else if (s1 == s2 || s1 == s3 || s2 == s3) sum+=8;   // 2 suited

	// connectors
	int diff3 = abs(r1-r2);
	int diff2 = abs(r1-r2);
	int diff1 = abs(r2-r3);
	if (0<diff3 && diff3<5) sum+=(5-diff3)/2;  // smaller difference = better, divide by 2
	if (0<diff2 && diff2<5) sum+=(5-diff2)/2;
	if (0<diff1 && diff1<5) sum+=(5-diff1)/2;

	return sum/6-2; // how much to bet to
}


int Player::compute_betsize_model2(Card c1, Card c2, Card c3) {

	int r1, r2, r3;
	r1 = rank_to_int(c1.rank);
	r2 = rank_to_int(c2.rank);
	r3 = rank_to_int(c3.rank);
	
	char s1, s2, s3;
	s1 = c1.suit;
	s2 = c2.suit;
	s3 = c3.suit;

	int sum = r1 + r2 + r3;
	if (r1 == r2 && r1 == r3) return sum;           		 // 3 of a kind
	else if (r1 == r2 || r1 == r3 || r2 == r3) sum+=13;  // 2 of a kind

	if (s1 == s2 && s1 == s3) sum+=4;                    // 3 suited
	else if (s1 == s2 || s1 == s3 || s2 == s3) sum+=8;   // 2 suited

	// connectors
	int diff3 = abs(r1-r2);
	int diff2 = abs(r1-r2);
	int diff1 = abs(r2-r3);
	if (0<diff3 && diff3<5) sum+=(5-diff3);
	if (0<diff2 && diff2<5) sum+=(5-diff2);
	if (0<diff1 && diff1<5) sum+=(5-diff1);

	return sum/6;
}

//double Player::preflop_raise_stats()


void Player::create_equity250() {	

}
