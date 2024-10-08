//
//  defs.h
//  strawberrymangoes
//
//  Created by Max Smiley on 9/20/20.
//  Copyright © 2020 Max Smiley. All rights reserved.
//

#ifndef defs_h
#define defs_h

#include <iostream>
#include <fstream>

#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <pthread.h>
#include <utility>

class Board;

//Pieces
#define OFFBOARD -1
#define WP 0
#define WN 1
#define WB 2
#define WR 3
#define WQ 4
#define WK 5
#define BP 6
#define BN 7
#define BB 8
#define BR 9
#define BQ 10
#define BK 11
#define NOPIECE 12

//Castling Perms
#define INVERSE 15  //inverts castling bits when XORed
#define WKCA 8
#define WQCA 4
#define BKCA 2
#define BQCA 1

//Sides
#define WHITE 0
#define BLACK 1
#define NEITHER 2

//Misc
#define BISHOP_PAIR 20
#define NOMOVE 0
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 32
#define MAXGAMELENGTH 1024
#define TTABLEENTRIES 100000

#define MATE 90000
#define INFINITE 100000

//A quick shortcut, piece representations indexed by piece type
static std::string PceChar = "PNBRQKpnbrqk.";
static std::string RankChar = "87654321";
static std::string FileChar = "abcdefgh";

//If true, calling functions that print to cout will include extra information
static bool debugMode = false;

/*
 If true, the computer will monitor the text file specified. It will wait until a move is written, and it will play that move, calculate, and output a move into the given file. To setup a game between two AIs, we must have two instances of the engine running listening to the same port, have them be opposite colors and have the same start FEN.
 */
static bool computerMode = false;
static std::string inSocket = "";
static std::string outSocket = "";

//When turned on, the computer will use the player's time to perform calculations
static bool ponderMode = false;

int PceCol (int pce);   //Returns color, given a piece
int RAND32 ();          //Returns a random 32 bit integer

bool isPawn(int pce);   //returns true if pawn
bool isKing(int pce);   //returns true if king
bool doDiag(int pce);   //return true if bishop or queen
bool doVert(int pce);   //return trie if rook or queen

//Related to extracting information about the move key
int fromR(int move);
int fromC(int move);
int toR(int move);
int toC(int move);
int captured(int move);
int promoted(int move);
bool isEnpasMove(int move);
bool isPawnstartMove(int move);
bool isCastleMove(int move);
std::string printMove(int move);    //converts move e2e4 form

int worth(int pce);     //returns worth, give piece

//static evaluation, to be called at the leaf nodes of the game tree
const int static_eval(const Board& b);

//Clears the command line screen
void clearScreen();

//Timer class for statistics purposes
class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {
        std::chrono::duration<double,std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

//For the history table
struct History
{
    int posKey;
    int move;
    int pce;
    int castlePerm;
    int enpasSquareR;
    int enpasSquareC;
    int fiftyMove;
};

//For the searcher class
struct PVNode
{
    int posKey;
    int move;
    int score;
};

//Caches priority of move so we don't have re compute this
struct Move
{
    int move;
    int priority;
};

//Statistics collection for move ordering and nodes visited info
struct Stats
{
    void reset()
    {
        nodes = 0;
        failHigh = 0;
        failHighFirst = 0;
    }
    
    int nodes;
    int failHigh;
    int failHighFirst;
};

//Transposition table
struct Trans
{
    int pos;
    int score;
    int depth;
};
#endif /* defs_h */
