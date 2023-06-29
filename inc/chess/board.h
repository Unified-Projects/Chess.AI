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
struct MoveExtra{ // TODO: Maybe integrate to make a better Promotion system or try to remove
    int type = 0;

    int x = 0;
    int y = 0;

    Piece* change = nullptr;
};

// For caching previous moves
struct MoveCache{
    // Pieces
    Piece* MovedPiece;
    Piece* TargetPiece;

    // Positions
    int StartX;
    int StartY;
    int EndX;
    int EndY;

    MoveExtra Extra;
};

class Board {
    friend struct Piece;
public: // Standardised
    // Define standard map to FEN kesy to our defined pieces
    static std::map<char, Piece*> pieceMapper;
    static std::map<Type, char> typeMapper; // Usefull for the move notation and checking if it is valid

protected: // Private board management
    Piece* board[8][8] = {};

    void SetPiece(int X, int Y, Piece* p);

    // Check management
    bool Check = false;
    bool Stale = false;
    bool Mate = false;

    // Color
    Colour CheckedColour = NULL_COLOUR;

    // King savings
    Piece* WhiteKing = nullptr;
    Piece* BlackKing = nullptr;

    // Pieces
    std::list<Piece*> WhitePieces = {};
    std::list<Piece*> BlackPieces = {};

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
    bool PlayMove(std::string notation) {UNIMPLEMENTED return false;}; // Try to use standard notations //TODO: Pipe into MovePiece after translating
    bool MovePiece(int startX, int startY, int endX, int endY, bool ignoreCheck=false); // Simpler moving function
    void UndoMove(); // Simple undo previous move

    // Check Updater
    bool UpdateCheck();
    bool UpdateCheckmate();
    bool UpdateStalemate();
    bool IsCheck(){return Check;}

    // Get Pieces
    std::list<Piece*> GetWhitePieces() {return std::list<Piece*>(this->WhitePieces);}
    std::list<Piece*> GetBlackPieces() {return std::list<Piece*>(this->BlackPieces);}

    // Current Played Moves
    std::vector<MoveCache> PlayedMoves;

    // 
    std::list<std::pair<int, int>> MoveGen(Type t);

public: // Other
    std::string ConvertToFen();

public: // Debuggers
    void LogBoard(); // Print board to console
};