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

// TODO CHANGES
/*
    Bit Layout of piece in a board:
    
    The piece type in first 3 bits (Follows Enum)
    0 NULL Or Not
    0      1        0      1        0       1
    0 Pawn 0 Bishop 1 Rook 1 Knight 0 Queen 0 King
    0      0        0      0        1       1
        0 White 1 Black
            0 Movements::SLIDE
            0 Movements::SLIDE_HORIZONTAL
            0 Movements::SLIDE_DIAGONAL
            0 Movements::PAWN_MOVMENT
            0 Movements::KNIGHT_MOVMENT
            0 Movements::KING_MOVMENT
            0 Movements::EN_PASSENT // TODO: Enable when within piece range to not need to keep using If statements
            UNUSED // 0 Movements::CASTLING // TODO: use to cancel castling capabilites when moved
            INSTEAD
            0 Has Moved // More efficient and easier
                0 // Unused
                0 // Unused
                0 // Unused

    A total of 13 bits used to store this data
    Allowing the entire board to be represented as a short (16-bit value)

    // Notes
    We no longer store the pieces current square position so we may need to adjust stuff to give the information

    // Undo move easier
    Since we no longer manage pointers and shit
    We can just cache the board and then restore the array pointer to board
    So no management of shitty MoveExtras and stuff

    // Check Management (Maybe not idk)
    We hold a bitmap (uint64_t) and use bit masks to access each individual bit
    It will mention the spaces on the board that are being attacked

    // Move lists
    We can store both colours in separate lists so that we can always access the others moves
*/

// Accessors as Macros (Not functions as to be efficient)
#define IsNull(piece) piece & 0b0000000000000001 // First Bit
#define GetType(piece) piece & 0b0000000000001110 // Next 3 bits
#define GetColour(piece) piece & 0b0000000000010000 // Next 1 bits
#define GetCapabilites(piece, capability) (piece >> 4) & capability // Next 7 bits
#define GetMoved(piece) piece & 0b0001000000000000 // Next 1 bits
#define GetUnused(piece) piece & 0b1110000000000000 // Next 3 bits

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