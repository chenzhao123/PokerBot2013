#include <iostream>
#include "player.hpp"
#include <stdlib.h>
#include <cmath>
#include "pbots_calc.h"

Player::Player() {
    //create_equity250();
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
        std::string request_keyvalue_action("REQUESTKEYVALUES");
        std::string newgame("NEWGAME");
        std::string keyvalue("KEYVALUE");
        std::string newhand("NEWHAND");
        std::string handover("HANDOVER");
        std::string first_word = line.substr(0, line.find_first_of(' '));
        if (newgame.compare(first_word) == 0) {
            new_game(line.substr(line.find_first_of(' ')+1));
        } else if (getaction.compare(first_word) == 0) {
            get_action(line.substr(line.find_first_of(' ')+1));
            get_action_respond(stream);
        } else if (request_keyvalue_action.compare(first_word) == 0) {
            // FINISh indicates no more keyvalue pairs to store.
            stream << "FINISH\n";
        } else if (keyvalue.compare(first_word) == 0){

        } else if (newhand.compare(first_word) == 0){
            new_hand(line.substr(line.find_first_of(' ')+1));
        } else if (handover.compare(first_word) == 0){
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

        action_list.push_back(act);    // add act to action_list
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

    space_index    = line.find_first_of(' ');
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

        action_list.push_back(act);    // add act to action_list
        line = line.substr(space_index+1);
    }

    time_bank = atof(line.c_str());
    action_list.clear();
}

void Player::get_action_respond(tcp::iostream &stream){
    calculate_equity_random_opp();

    if (num_boardcards == 0){         // Pre-flop
        if (my_equity > 0.58) {
            LegalAction raise_act;
            raise_act.action = "RAISE";
            if (legal_actions.find(raise_act) != legal_actions.end()){
                LegalAction act = *(legal_actions.find(raise_act));
                int amt = 20 * my_equity + ((rand() % 7) - 3);
                if (amt > act.max) amt = act.max;
                else if (amt < act.min) amt = act.min;
                stream << "RAISE:" << amt << "\n";
            } else stream << "CALL\n";
        } else if(my_equity > 0.5) {
            PerformedAction last_action = *(action_list.end()-1);
            if (last_action.action.compare("RAISE") == 0) {
                int raise_amt = last_action.amount;
                if (raise_amt < 20*my_equity) stream << "CALL\n";
                else stream << "CHECK\n"; // Will auto fold if check is illegal
            } else stream << "CALL\n";

        }
        else stream << "CHECK\n";  // Will auto fold if check is illegal
        //get_preflop_action(stream);

    } else if (num_boardcards == 3) { // Flop
        LegalAction discard_act;
          discard_act.action = "DISCARD";
          if (legal_actions.find(discard_act) != legal_actions.end()){
            stream << "DISCARD:" << discarded.rank << discarded.suit << "\n";
            return;
          }

        if (my_equity > 0.67) stream << "CALL\n";
        else stream << "CHECK\n"; //Will auto fold if check is illegal


    } else if (num_boardcards == 4) { // Turn
        if (my_equity > 0.74) stream << "CALL\n";
        else {
          PerformedAction last_action = *(action_list.end()-1);
          int opp_raise = 0;
          int current_pot = 0;
            if ((last_action.action).compare("BET") == 0) {
            opp_raise = last_action.amount;
            current_pot = pot_size - opp_raise;
            }
            /*
            else if ((last_action.action).compare("RAISE") == 0) {
            opp_raise = last_action.amount;
            current_pot = pot_size - opp_raise;
            }
            */
            if (current_pot > 400) stream << "CALL\n";
          else stream << "CHECK\n"; //Will auto fold if check is illegal
        }
        //get_postflop_action(stream);

    } else if (num_boardcards == 5) { // River
        if (my_equity > 0.81) {
          LegalAction bet_act;
          LegalAction raise_act;
          bet_act.action = "BET";
          raise_act.action = "RAISE";
          if (legal_actions.find(bet_act) != legal_actions.end()){
            LegalAction act = *(legal_actions.find(bet_act));
            stream << "BET:" << act.max << "\n";
          } else if (legal_actions.find(raise_act) != legal_actions.end()){
            LegalAction act = *(legal_actions.find(raise_act));
            stream << "RAISE:" << act.max << "\n";
          } else {
              stream << "CALL\n";
          }
        } else {
          PerformedAction last_action = *(action_list.end()-1);
          int opp_raise = 0;
          int current_pot = 0;
            if ((last_action.action).compare("BET") == 0) {
            opp_raise = last_action.amount;
            current_pot = pot_size - opp_raise;
            }
            /*
            else if ((last_action.action).compare("RAISE") == 0) {
            opp_raise = last_action.amount;
            current_pot = 2*(pot_size - opp_raise);
            }
            */
            if (current_pot > 400) stream << "CALL\n";
          else stream << "CHECK\n"; //Will auto fold if check is illegal
        }

    }
}



void Player::calculate_equity_random_opp(){

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
    char hand[] = {card1.rank, card1.suit, card2.rank, card2.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    char* myhand = hand;
    Results* res = alloc_results();
    calc(myhand, (char*)"", (char*)"", 4000, res);
    my_equity = *((res->ev)+0);
    opp_equity = *((res->ev)+1);
    free_results(res);

  } else if (num_boardcards == 3){ //Flop
    char hand1[] = {card1.rank, card1.suit, card2.rank, card2.suit, ':', 'x', 'x','\0'};
    char hand2[] = {card1.rank, card1.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};
    char hand3[] = {card2.rank, card2.suit, card3.rank, card3.suit, ':', 'x', 'x','\0'};


    char* myhand = hand1;
    char dead_card[] = {card3.rank, card3.suit,'\0'};
    char* dead = dead_card;
    Results* res1 = alloc_results();

    calc(myhand, board, dead, 4000, res1);


    myhand = hand2;
    dead_card[0] = card2.rank;
    dead_card[1] = card2.suit;
    Results* res2 = alloc_results();

    calc(myhand, board, dead, 4000, res2);

    myhand = hand3;
    dead_card[0] = card1.rank;
    dead_card[1] = card1.suit;
    Results* res3 = alloc_results();

    calc(myhand, board, dead, 4000, res3);

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
    calc(myhand, board, dead, 4000, res);
    my_equity = *((res->ev)+0);
    opp_equity = *((res->ev)+1);
    free_results(res);
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
