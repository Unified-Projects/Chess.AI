#pragma once
#include <string>
#include <iostream>

// Pre-Definitions
class Board;
struct MoveExtra;
struct Move;

// Pre-define piece
class Piece;

// Piece Management Constants
    extern const int DirectionalOffsets[];
    static int SquaresToEdge[64][24];
//

// Used to generate the movement vectors and distances
extern void PrecomputeEdges();

// Sliding move generator
extern void GenerateSlidingMoves(int Square, Piece* piece, Board* b);

// Pawn Movements
extern void GeneratePawnMovements(int Square, Piece* piece, Board* b);

// Knight Movements
extern void GenerateKnightMovements(int Square, Piece* piece, Board* b);

// King Movements
extern void GenerateKingMovements(int Square, Piece* piece, Board* b);

//Enums for piece configurations
    enum Colour{
        BLACK = 0x00,
        WHITE = 0x01,
        NULL_COLOUR = -0x01
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

    enum Movements{ // All fit into one byte :)
        // Sliding Moves
        SLIDE = 0x1,
        SLIDE_HORIZONTAL = 0x2,
        SLIDE_DIAGONAL = 0x4,

        // Other Pieces
        PAWN_MOVMENT = 0x08,
        KNIGHT_MOVMENT = 0x10,
        KING_MOVMENT = 0x20,

        // Special
        EN_PASSENT = 0x40,
        CASTLING = 0x80,
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
        // Keeping track, for castling and en-passent
        int moveCount;

        // Movements
        const char MovingCapabilites; // Read-Only !

        // Position
        int Square = -1;

        // Setup for board setup
        Piece(Colour c = NULL_COLOUR, const char MovingCapables = 0) : MovingCapabilites(MovingCapables) {this->c = c;moveCount=0;}

        // To return protected variables to non-board
        int GetCapabilites() {return MovingCapabilites;}
        Type GetT() {return t;}
        Colour GetC() {return c;}

        // For creating new pieces
        virtual Piece* Clone() {return new Piece(*this);}
    };
//

// Type specific definitions
    struct Pawn : public Piece{
    public:
        Pawn(Colour c) : Piece(c, PAWN_MOVMENT) {t=PAWN;return;}

        Piece* Clone() {return new Pawn(*this);}
    };

    struct Bishop : public Piece{
        friend class Board;
    public:
        Bishop(Colour c) : Piece(c, SLIDE + SLIDE_DIAGONAL) {t=BISHOP;return;}

        Piece* Clone() {return new Bishop(*this);}
    };

    struct Rook : public Piece{
        friend class Board;
    public:
        Rook(Colour c) : Piece(c, SLIDE + SLIDE_HORIZONTAL) {t=ROOK;return;}

        Piece* Clone() {return new Rook(*this);}
    };

    struct Knight : public Piece{
        friend class Board;
    public:
        Knight(Colour c) : Piece(c, KNIGHT_MOVMENT) {t=KNIGHT;return;}

        Piece* Clone() {return new Knight(*this);}
    };

    struct Queen : public Piece{
        friend class Board;
    public:
        Queen(Colour c) : Piece(c, SLIDE) {t=QUEEN;return;}

        Piece* Clone() {return new Queen(*this);}
    };

    struct King : public Piece{
        friend class Board;
    public:
        King(Colour c) : Piece(c, KING_MOVMENT) {t=KING;return;}

        Piece* Clone() {return new King(*this);}
    };
//