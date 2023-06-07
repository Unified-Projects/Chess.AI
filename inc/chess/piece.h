#pragma once
#include <string>

// External definition for friend classing
class Board;

//Enums for piece configurations
    enum Colour{
        BLACK = 0x00,
        WHITE = 0xFF,
        NULE = 0xFFF
    };

    enum Type{
        NULE_T = -1,
        PAWN = 0,
        BISHOP = 1,
        ROOK = 2,
        KNITGHT = 3,
        QUEEN = 4,
        KING = 5
    };
//

// Basic Piece definition
    struct Piece{
        friend class Board;
    protected:
        // Typing
        Type t = NULE_T;
        Colour c;

        // Parent referece
        Board* b;

    public:
        // Position vector
        int X;
        int Y;

        // Setup for board setup
        Piece(Colour c = NULE) {this->c = c;}

        // To return protected variables to non-board
        Type GetT() {return t;}
        Colour GetC() {return c;}

        // Predefine so that it can be customised per piece
        virtual bool ValidMove(Piece* Target) {return false;}
    };
//

// Type specific definitions
    struct Pawn : protected Piece{
        friend class Board;
    public:
        Pawn(Colour c) : Piece(c) {t=PAWN;return;}
        bool ValidMove(Piece* Target);
    };

    struct Bishop : protected Piece{
        friend class Board;
    public:
        Bishop(Colour c) : Piece(c) {t=BISHOP;return;}
        bool ValidMove(Piece* Target);
    };

    struct Rook : protected Piece{
        friend class Board;
    public:
        Rook(Colour c) : Piece(c) {t=ROOK;return;}
        bool ValidMove(Piece* Target);
    };

    struct Knight : protected Piece{
        friend class Board;
    public:
        Knight(Colour c) : Piece(c) {t=KNITGHT;return;}
        bool ValidMove(Piece* Target);
    };

    struct Queen : protected Piece{
        friend class Board;
    public:
        Queen(Colour c) : Piece(c) {t=QUEEN;return;}
        bool ValidMove(Piece* Target);
    };

    struct King : protected Piece{
        friend class Board;
    public:
        King(Colour c) : Piece(c) {t=KING;return;}
        bool ValidMove(Piece* Target);
    };
//