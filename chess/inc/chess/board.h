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
    SPECIAL_CASTLING = 2,
    SPECIAL_PROMOTION = 3
};
struct MoveExtra{ // TODO: Maybe integrate to make a better Promotion system or try to remove
    int type = 0;
    int square;

    // For caching
    Piece* From;
    Piece* To;
};

// Moves
struct Move{
    int Start;
    int End;

    Type Taking; // For check and mate

    MoveExtra Extra{};
};

// For caching previous moves
struct MoveCache{
    // Pieces
    Piece* MovedPiece;
    Piece* TargetPiece;

    // Positions
    Move move;

    MoveExtra Extra;
};

class Board {
    friend struct Piece;
public: // Standardised
    // Define standard map to FEN kesy to our defined pieces
    static std::map<char, Piece*> pieceMapper;
    static std::map<Type, char> typeMapper; // Usefull for the move notation and checking if it is valid

public: // Private board management
    Piece* board[64] = {};

    void SetPiece(int X, int Y, Piece* p);

    // Check management
    bool Check = false;
    bool Stale = false;
    bool Mate = false;

    // Color
    Colour CheckedColour = NULL_COLOUR;
    Colour CurrentMove = WHITE;

    // King savings
    Piece* WhiteKing = nullptr;
    Piece* BlackKing = nullptr;

    // Pieces
    std::list<Piece*> WhitePieces = {};
    std::list<Piece*> BlackPieces = {};

public: // Setup / De-setup
    // Probably should do something with me
    Board();

    // Almost entirely optional
    ~Board() {return;}

    // Setup using FEN (Not a constructor to make it easier to restart a game)
    void InitBoard(std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

public: // Gameplay
    // Get a piece from the board (Used by pieces for validation purposes)
    // Piece* GetPieceAtPosition(int X, int Y);
    // Piece* GetPieceAtPosition(char X, int Y);

    // // Play a move, independent of colour
    // bool PlayMove(std::string notation) {UNIMPLEMENTED return false;}; // Try to use standard notations //TODO: Pipe into MovePiece after translating
    // bool MovePiece(int startX, int startY, int endX, int endY, bool ignoreCheck=false); // Simpler moving function
    // void UndoMove(); // Simple undo previous move

    // // Check Updater
    // bool UpdateCheck();
    // bool UpdateCheckmate();
    // bool UpdateStalemate();
    // bool IsCheck(){return Check;}

    // // Get Pieces
    // std::list<Piece*> GetWhitePieces() {return std::list<Piece*>(this->WhitePieces);}
    // std::list<Piece*> GetBlackPieces() {return std::list<Piece*>(this->BlackPieces);}

    // std::list<std::pair<int, int>> MoveGen(Type t);

public: // Gameplay
    // Get piece
    Piece* GetSquare(int square){return board[square];}

    // Move generation
    std::list<Move> MoveList;
    Colour PreviousGeneration = NULL_COLOUR; // Caching
    std::list<Move> GenerateMoves();

    // Move Moving Moves
    bool MovePiece(Move m);
    void UndoMove();

    // Current Played Moves
    std::vector<MoveCache> PlayedMoves;

    // Game conditions
    bool UpdateCheck();
    bool UpdateMate();
    std::list<Move> Checkmoves;

public: // Other
    std::string ConvertToFen();

public: // Debuggers
    void LogBoard(); // Print board to console
};