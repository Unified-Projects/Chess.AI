#pragma once
#include <string>
#include <iostream>

// Pre-Definitions
class Board;
struct MoveExtra;
struct Move;

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

        // Operator to use index
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

void PrecomputeEdges();

// Sliding move generator
void GenerateSlidingMoves(int Square, Piece* piece, Board* b);

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

    enum Movements{

        // Sliding Moves
        SLIDE = 0x1,
        SLIDE_HORIZONTAL = 0x2,
        SLIDE_DIAGONAL = 0x4
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

        // Movements
        int MovingCapabilites;

    public:
        // Position vector
        int X;
        int Y;

        // Keeping track, for castling and en-passent
        int moveCount;

        // Setup for board setup
        Piece(Colour c = NULL_COLOUR) {this->c = c;X=0;Y=0;moveCount=0;}

        // To return protected variables to non-board
        int GetCapabilites() {return MovingCapabilites;}
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
        std::list<std::pair<int,int>> isValidMove(int targetX, int targetY, MoveExtra* Extra = nullptr);

        Piece* Clone() {return new King(*this);}
    };
//