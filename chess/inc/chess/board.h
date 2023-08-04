#pragma once
#include <string>
#include <chess/piece.h>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <algorithm>
#include <iostream>

// TEMPORARY INCLUDES
    #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

// Extras for special moves
enum SpecialMoveType{
    SPECIAL_EN_PASSENT = 1,
    SPECIAL_CASTLING_KING = 2,
    SPECIAL_CASTLING_QUEEN = 3,
    SPECIAL_PROMOTION = 4
};
struct MoveExtra{ // TODO: Maybe integrate to make a better Promotion system or try to remove
    int Type = 0;
    int Square;
};

// Moves
struct Move{
    int Start;
    int End;

    Type Taking; // For check and mate

    MoveExtra Extra{};
};

class Board {
    friend struct Piece;
public: // Standardised
    // Define standard map to FEN kesy to our defined pieces
    static std::map<char, Type> pieceMapper;
    static std::map<Type, char> typeMapper; // Usefull for the move notation and checking if it is valid

public: // Private board management
    short* board;

    // Check management
    bool Check = false;
    bool Stale = false;
    bool Mate = false;

    // Color
    Colour CheckedColour = NULL_COLOUR;
    Colour CurrentMove = WHITE;

    // King savings
    int WhiteKing = -1;
    int BlackKing = -1;

    // // Pieces
    std::list<int> WhitePieces = {};
    std::list<int> BlackPieces = {};

public: // Setup / De-setup
    // Probably should do something with me
    Board();

    // Almost entirely optional
    ~Board() {return;}

    // Setup using FEN (Not a constructor to make it easier to restart a game)
    void InitBoard(std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

public: // Gameplay
    // Get piece
    short GetSquare(int square){return board[square];}

    // Move generation
    std::list<Move> MoveList;
    Colour PreviousGeneration = NULL_COLOUR; // Caching
    std::list<Move> GenerateMoves();

    // Move Moving Moves
    bool MovePiece(Move m);
    void UndoMove();

    // Current Played Moves
    std::vector<Move> PlayedMoves;
    std::vector<short*> Boards;
    std::vector<std::list<int>> WhiteLists;
    std::vector<std::list<int>> BlackLists; // TODO: Some day,

    // Game conditions
    bool UpdateCheck();
    bool UpdateMate();
    std::list<Move> Checkmoves;

public: // Other
    std::string ConvertToFen();

public: // Debuggers
    void LogBoard(); // Print board to console
};