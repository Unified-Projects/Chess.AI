#pragma once
#include <string>
#include <iostream>

// Pre-Definitions
class Board;
struct Move;

// Piece Management Constants
    extern const int DirectionalOffsets[];
    static int SquaresToEdge[64][24];
//

// Used to generate the movement vectors and distances
extern void PrecomputeEdges();

// Sliding move generator
extern void GenerateSlidingMoves(int Square, Board* b);

// Pawn Movements
extern void GeneratePawnMovements(int Square, Board* b);

// Knight Movements
extern void GenerateKnightMovements(int Square, Board* b);

// King Movements
extern void GenerateKingMovements(int Square, Board* b);

//Enums for piece configurations
    enum Colour{
        BLACK = 0x00,
        WHITE = 0x01,
        NULL_COLOUR = 0x2
    };

    enum Type{
        PAWN = 0,
        BISHOP = 1,
        ROOK = 2,
        KNIGHT = 3,
        QUEEN = 4,
        KING = 5,
        NULL_TYPE = 8
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
        EN_PASSENT = 0x40, // TODO: FIX SPELLING EN_PASSANT
        CASTLING = 0x80,
    };
//

// TODO CHANGES
/*
    Bit Layout of piece in a board:

    The piece type in first 4 bits (Follows Enum, except negative)
    0      1        0      1        0       1
    0 Pawn 0 Bishop 1 Rook 1 Knight 0 Queen 0 King
    0      0        0      0        1       1
    0 NULL Or Not
        0 White 1 Black
        0 Null Colour
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
#define GetType(piece) Type((piece & 0b0000000000001111)) // First 4 bits
#define IsNull(piece) piece & NULL_TYPE // 4th Bit
#define GetColour(piece) Colour((piece & 0b0000000000110000) >> 4) // Next 2 bits
#define GetMoveCapabilites(piece, capability) (piece >> 6) & capability // Next 7 bits
#define GetMoved(piece) (piece & 0b0010000000000000) >> 13 // Next 1 bits
#define GetUnused(piece) piece & 0b1100000000000000 // Next 2 bits

// Piece Configurations (Mutator Macros)

#define PIECE_TYPE_SHIFT 0
#define PIECE_COLOR_SHIFT 4
#define MOVEMENT_SHIFT 6
#define HAS_MOVED_SHIFT 13

// Set type of piece
#define SetPieceType(value, type) value = ((value & ~(0b1111)) | ((type & 0b1111)))

// Set Null flag (0 for not null, 1 for null)
#define SetNullFlag(value, flag) value = ((value & ~(0x1 << NULL_FLAG_SHIFT)) | ((flag & 0x1) << NULL_FLAG_SHIFT))

// Set colour of piece (0 for white, 1 for black)
#define SetPieceColour(value, color) value = ((value & ~(0x1 << PIECE_COLOR_SHIFT)) | ((color & 0x3) << PIECE_COLOR_SHIFT))

// Set movement flags
#define SetMovements(value, movement) value = ((value & ~(0x7F << MOVEMENT_SHIFT)) | ((movement & 0x7F) << MOVEMENT_SHIFT))

// Set Has Moved flag (1 for moved, 0 for not moved)
#define SetHasMoved(value, flag) value = ((value & ~(0x1 << HAS_MOVED_SHIFT)) | ((flag & 0x1) << HAS_MOVED_SHIFT))

// Macros for setting piece to each type
#define SetPieceInfo(value, type, color, movement) do { \
    SetPieceType(value, type); \
    SetPieceColour(value, color); \
    SetMovements(value, movement); \
} while(0)

#define SET_NULL(value) SetPieceInfo(value, NULL_TYPE, NULL_COLOUR, 0x00)
#define SET_PAWN(value, color) SetPieceInfo(value, PAWN, color, PAWN_MOVMENT)
#define SET_BISHOP(value, color) SetPieceInfo(value, BISHOP, color, (SLIDE | SLIDE_DIAGONAL))
#define SET_ROOK(value, color) SetPieceInfo(value, ROOK, color, (SLIDE | SLIDE_HORIZONTAL))
#define SET_KNIGHT(value, color) SetPieceInfo(value, KNIGHT, color, KNIGHT_MOVMENT)
#define SET_QUEEN(value, color) SetPieceInfo(value, QUEEN, color, SLIDE)
#define SET_KING(value, color) SetPieceInfo(value, KING, color, (KING_MOVMENT | SLIDE)) // We give sliding stuff for check

#define SET_MOVED(value) SetHasMoved(value, 1)