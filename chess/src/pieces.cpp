#include <chess/piece.h>
#include <chess/board.h>

// Piece Management Constants
    const int DirectionalOffsets[] = {
        8, -8,
        -1, 1,
        7, -7,
        9, -9,

        // Knight
        6, -6,
        10, -10,
        15, -15,
        17, -17,

        // Pawn
        7, 8,
        9, 16,
        -7, -8,
        -9, -16
    };
//

// To run at the begining
void PrecomputeEdges(){
    static bool Ran;

    if(Ran){
        return;
    }

    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            // Generate directional data
            int North = 7 - y;
            int South = y;
            int West = x;
            int East = 7 - x;

            // Calculate index
            int Index = (y * 8) + x;

            // Knight calculations
            int knightStuff[8];

            for (int i = 8; i < 16; i++){
                int pos = Index + DirectionalOffsets[i];

                // Ensures a overall L shape
                // Pre compute gradients
                int yChange = abs((int)((Index + DirectionalOffsets[i]) / 8) - y);
                int xChange = abs((int)((Index + DirectionalOffsets[i]) % 8) - x);

                // Now ensure gradient is constant
                if(yChange/xChange != 2/1 && yChange/xChange != 1/2){
                    knightStuff[i-8] = 0;
                    continue;
                }

                knightStuff[i-8] = (pos <= 63 && pos >= 0 && (xChange == 2 && yChange == 1 || xChange == 1 && yChange == 2));
            }

            // Insert data
            int ComputedData[24] = {
                North,
                South,
                West,
                East,

                // Extras
                std::min(North, West),
                std::min(South, East),
                std::min(North, East),
                std::min(South, West),

                // Knights
                knightStuff[0],
                knightStuff[1],
                knightStuff[2],
                knightStuff[3],
                knightStuff[4],
                knightStuff[5],
                knightStuff[6],
                knightStuff[7],

                // Pawns
                (y != 7) && (West > 0),
                (y != 7),
                (y != 7) && (East > 0),
                (y == 1),
                (y != 0) && (East > 0),
                (y != 0),
                (y != 0) && (West > 0),
                (y == 6)
            };

            memcpy(SquaresToEdge[Index], ComputedData, 4*24);
        }
    }

    // Restore
    Ran = true;
    return;
}

// Used for queens, bishops, rooks
void GenerateSlidingMoves(int Square, Board* b){
    short piece = b->board[Square];

    // Compatible directions
    int StartIndex = (GetMoveCapabilites(piece, SLIDE_DIAGONAL)) ? 4 : 0;
    int EndIndex = (GetMoveCapabilites(piece, SLIDE_HORIZONTAL)) ? 4 : 8;

    char Capabs = 0x00;

    if(GetMoveCapabilites(piece, SLIDE_DIAGONAL)){
        Capabs += SLIDE_DIAGONAL;
    }
    else if(GetMoveCapabilites(piece, SLIDE_HORIZONTAL)){
        Capabs += SLIDE_HORIZONTAL;
    }

    Capabs += SLIDE;

    // Check all directions possible
    for (int directionI = StartIndex; directionI < EndIndex; directionI++){
        for(int nToEdge = 0; nToEdge < SquaresToEdge[Square][directionI]; nToEdge++){
            int targetSquare = Square + (DirectionalOffsets[directionI] * (nToEdge + 1));
            short targetPiece = b->board[targetSquare];

            // Blocked by friend
            if(GetColour(targetPiece) == GetColour(piece)){
                break; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, Capabs});

            // If attacking place
            if(GetColour(targetPiece) != GetColour(piece) && GetColour(targetPiece) != NULL_COLOUR){
                break; // Enemy encountered, end of direction
            }
        }
    }
}

// Pawn Movements
void GeneratePawnMovements(int Square, Board* b){
    short piece = b->board[Square];

    // Direction based on colour
    int StartIndex = (GetColour(piece) == WHITE) ? 16 : 20;

    // TODO: Optimise!
    for (int directionI = StartIndex; directionI < StartIndex + 4; directionI += 1){
        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int offset = DirectionalOffsets[directionI];
        int targetSquare = Square + offset;
        short targetPiece = b->board[targetSquare];

        if(GetColour(targetPiece) == GetColour(piece)){ // No Attacky Fweind
            continue;
        }

        if (abs(offset) == 8) {
            // Blocked by friend
            if(GetType(targetPiece) != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            // Now see if this is a promotion issue
            if(targetSquare >= ((GetColour(piece) == WHITE) ? 56 : 0) && targetSquare <= ((GetColour(piece) == WHITE) ? 63 : 7)){ // Time to promote
                // TODO PICKER?
                b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), MoveExtra{SPECIAL_PROMOTION, targetSquare}, PAWN_MOVMENT});
            }
            else{ // Norm
                b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, PAWN_MOVMENT});
            }
        }
        else if (abs(offset) == 16) {
            // Blocked by friend
            if(GetType(targetPiece) != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            int intermiateSquare = Square + (offset / 2);
            short intermiatePiece = b->board[intermiateSquare];

            // Blocked by friend
            if(GetType(intermiatePiece) != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            // Blocked by friend
            if(GetType(targetPiece) != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            // Now see if this is a promotion issue
            if(targetSquare >= ((GetColour(piece) == WHITE) ? 56 : 0) && targetSquare <= ((GetColour(piece) == WHITE) ? 63 : 7)){ // Time to promote
                // TODO PICKER?
                b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), MoveExtra{SPECIAL_PROMOTION, targetSquare}, PAWN_MOVMENT});
            }
            else{ // Norm
                b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, PAWN_MOVMENT});
            }
        }
        else if (abs(offset) == 7 || abs(offset) == 9) {
            // Check for en-passent
            if(GetType(targetPiece) == NULL_TYPE){
                // En-passent check
                int EnPassentSquare = targetSquare + ((GetColour(piece) == WHITE) ? -8 : 8);
                // TODO: Incorrect Ammount but somehow adds 3000 moves
                if(Square >= ((GetColour(piece) == WHITE) ? 32 : 24) && Square <= ((GetColour(piece) == WHITE) ? 39 : 31)){ // En-passent possible
                    if(GetMoved(b->board[EnPassentSquare]) && b->PlayedMoves.back().End == EnPassentSquare && b->PlayedMoves.back().Start == EnPassentSquare + ((GetColour(piece) == WHITE) ? 16 : -16) && GetType(b->board[EnPassentSquare]) == PAWN){
                        b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {SPECIAL_EN_PASSENT, EnPassentSquare}, PAWN_MOVMENT + EN_PASSENT});
                    }
                }
            }
            else if(GetColour(targetPiece) != GetColour(piece) && GetType(targetPiece) != NULL_TYPE) {
                // rEGULAR dINGNAL mOVEMENT
                // all good

                // Now see if this is a promotion issue
                if(targetSquare >= ((GetColour(piece) == WHITE) ? 56 : 0) && targetSquare <= ((GetColour(piece) == WHITE) ? 63 : 7)){ // Time to promote
                    // TODO: PICKER?
                    b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), MoveExtra{SPECIAL_PROMOTION, targetSquare}, PAWN_MOVMENT});
                }
                else{ // Norm
                    b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, PAWN_MOVMENT});
                }
            }
        }
    }
}

// Knight Movements
void GenerateKnightMovements(int Square, Board* b){
    short piece = b->board[Square];

    for(int directionI = 8; directionI < 16; directionI++){
        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int targetSquare = Square + DirectionalOffsets[directionI];
        short targetPiece = b->board[targetSquare];

        // Blocked by friend
        if(GetColour(targetPiece) == GetColour(piece)){
            continue; // Wrong, so change direction
        }

        b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, KNIGHT_MOVMENT});
    }
}

// King Movements
void GenerateKingMovements(int Square, Board* b){
    short piece = b->board[Square];

    // Check all directions possible
    for (int directionI = 0; directionI < 8; directionI++){

        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int targetSquare = Square + DirectionalOffsets[directionI];
        short targetPiece = b->board[targetSquare];

        // Blocked by friend
        if(GetColour(targetPiece) == GetColour(piece)){
            continue; // Wrong, so change direction
        }

        b->MoveList.push_back(Move{Square, targetSquare, GetType(targetPiece), {0}, KING_MOVMENT});
    }

    // TODO: Castling More checks needed so not perfect
    // Cant go through check
    // Cant go into check
    if (!GetMoved(piece)) {
        // Offset of -2

        // Check if rook is there
        if (GetType(b->board[Square-4]) == ROOK && !GetMoved(b->board[Square-4])) {
            // Check if path is clear
            if (GetType(b->board[Square-3]) == NULL_TYPE && GetType(b->board[Square-2]) == NULL_TYPE && GetType(b->board[Square-1]) == NULL_TYPE) {
                b->MoveList.push_back(Move{Square, Square-2, NULL_TYPE, {SPECIAL_CASTLING_QUEEN, Square-4}, KING_MOVMENT});
            }
        }

        // Offset of +2

        // Check if rook is there
        if (GetType(b->board[Square+3]) == ROOK && !GetMoved(b->board[Square+3])) {
            // Check if path is clear
            if (GetType(b->board[Square+1]) == NULL_TYPE && GetType(b->board[Square+2]) == NULL_TYPE) {
                b->MoveList.push_back(Move{Square, Square+2, NULL_TYPE, {SPECIAL_CASTLING_KING, Square+3}, KING_MOVMENT});
            }
        }
    }
}