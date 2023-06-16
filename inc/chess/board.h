#pragma once
#include <string>
#include <chess/piece.h>
#include <stdint.h>
#include <map>
#include <algorithm>
#include <iostream>

// TEMPORARY INCLUDES
    #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

class Board {
protected: // Standardised
    // Define standard map to FEN kesy to our defined pieces
    static std::map<char, Piece*> pieceMapper;
    static std::map<char, Type> typeMapper; // Usefull for the move notation and checking if it is valid

protected: // Private board management
    Piece* board[8][8] = {};

    void SetPiece(int X, int Y, Piece* p);

    // Check management
    bool WhiteCheck = false;
    bool BlackCheck = false;
    
    // King savings
    Piece* WhiteKing = nullptr;
    Piece* BlackKing = nullptr;

public: // Setup / De-setup
    // Probably should do something with me
    Board() {return;}

    // Almost entirely optional
    ~Board() {return;}

    // Setup using FEN (Not a constructor to make it easier to restart a game)
    void InitBoard(std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

public: // Gameplay
    // Get a piece from the board (Used by pieces for validation purposes)
    Piece* GetPieceAtPosition(int X, int Y);
    Piece* GetPieceAtPosition(char X, int Y);

    // Play a move, independent of colour
    bool PlayMove(std::string notation) {UNIMPLEMENTED return false;}; // Try to use standard notations
    bool MovePiece(int startX, int startY, int endX, int endY); // Simpler moving function TODO: REMOVE AND REPLACE WITH BETTER PlayMove

public: // Debuggers
    void LogBoard(); // Print board to console
};