#pragma once
#include <string>
#include <iostream>

// Pre-Definitions
class Board;
struct MoveExtra;

//Enums for piece configurations
    enum Colour{
        BLACK = 0x00,
        WHITE = 0xFF,
        NULL_COLOUR = 0xFFF
    };

    enum Type{
        NULL_TYPE = -1,
        PAWN = 0,
        BISHOP = 1,
        ROOK = 2,
        KNIGHT = 3,
        QUEEN = 4,
        KING = 5
    };
//

// Basic Piece definition
    struct Piece{
        friend class Board;
    protected:
        // Typing
        Type t = NULL_TYPE;
        Colour c = NULL_COLOUR;

        // Parent referece
        Board* b;

    public:
        // Position vector
        int X;
        int Y;

        // Keeping track, for castling and en-passent
        int moveCount;

        // Setup for board setup
        Piece(Colour c = NULL_COLOUR) {this->c = c;X=0;Y=0;moveCount=0;}

        // To return protected variables to non-board
        Type GetT() {return t;}
        Colour GetC() {return c;}

        // Predefine so that it can be customised per piece
        virtual bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr) {return false;}

        // For creating new pieces
        virtual Piece* Clone() {return new Piece(*this);}
    };
//

// Type specific definitions
    struct Pawn : public Piece{
    public:
        Pawn(Colour c) : Piece(c) {t=PAWN;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new Pawn(*this);}
    };

    struct Bishop : protected Piece{
        friend class Board;
    public:
        Bishop(Colour c) : Piece(c) {t=BISHOP;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new Bishop(*this);}
    };

    struct Rook : protected Piece{
        friend class Board;
    public:
        Rook(Colour c) : Piece(c) {t=ROOK;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new Rook(*this);}
    };

    struct Knight : protected Piece{
        friend class Board;
    public:
        Knight(Colour c) : Piece(c) {t=KNIGHT;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new Knight(*this);}
    };

    struct Queen : protected Piece{
        friend class Board;
    public:
        Queen(Colour c) : Piece(c) {t=QUEEN;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new Queen(*this);}
    };

    struct King : protected Piece{
        friend class Board;
    public:
        King(Colour c) : Piece(c) {t=KING;return;}
        bool isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new King(*this);}
    };
//