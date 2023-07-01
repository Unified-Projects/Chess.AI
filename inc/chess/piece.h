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
    struct EdgeInfo
    {
        int North = 0;
        int South = 0;
        int West = 0;
        int East = 0;

        // Extras
        int MinNW = 0;
        int MinSE = 0;
        int MinNE = 0;
        int MinSW = 0;

        // Operator to use indexing to get the values, due to non-edit restriction
         // TODO in the pregenerate modify indexeds of ints to remove this need
        int operator [](int index){
            switch (index)
            {
            case 0:
                return North;
            case 1:
                return South;
            case 2:
                return West;
            case 3:
                return East;
            case 4:
                return MinNW;
            case 5:
                return MinSE;
            case 6:
                return MinNE;
            case 7:
                return MinSW;
        
            default:
                return 0;
            }
        }
    };
    

    extern const int DirectionalOffsets[];
    static EdgeInfo SquaresToEdge[64];
//

// Used to generate the movement vectors and distances
extern void PrecomputeEdges();

// Sliding move generator
extern void GenerateSlidingMoves(int Square, Piece* piece, Board* b);

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

    struct Bishop : protected Piece{
        friend class Board;
    public:
        Bishop(Colour c) : Piece(c, SLIDE + SLIDE_DIAGONAL) {t=BISHOP;return;}

        Piece* Clone() {return new Bishop(*this);}
    };

    struct Rook : protected Piece{
        friend class Board;
    public:
        Rook(Colour c) : Piece(c, SLIDE + SLIDE_HORIZONTAL) {t=ROOK;return;}

        Piece* Clone() {return new Rook(*this);}
    };

    struct Knight : protected Piece{
        friend class Board;
    public:
        Knight(Colour c) : Piece(c, KNIGHT_MOVMENT) {t=KNIGHT;return;}

        Piece* Clone() {return new Knight(*this);}
    };

    struct Queen : protected Piece{
        friend class Board;
    public:
        Queen(Colour c) : Piece(c, SLIDE + SLIDE_HORIZONTAL + SLIDE_DIAGONAL) {t=QUEEN;return;}

        Piece* Clone() {return new Queen(*this);}
    };

    struct King : protected Piece{
        friend class Board;
    public:
        King(Colour c) : Piece(c, KING_MOVMENT) {t=KING;return;}

        Piece* Clone() {return new King(*this);}
    };
//