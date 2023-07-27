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
void GenerateSlidingMoves(int Square, Piece* piece, Board* b){
    // Compatible directions
    int StartIndex = (piece->MovingCapabilites & SLIDE_DIAGONAL) ? 4 : 0;
    int EndIndex = (piece->MovingCapabilites & SLIDE_HORIZONTAL) ? 4 : 8;

    // Check all directions possible
    for (int directionI = StartIndex; directionI < EndIndex; directionI++){
        for(int nToEdge = 0; nToEdge < SquaresToEdge[Square][directionI]; nToEdge++){
            int targetSquare = Square + (DirectionalOffsets[directionI] * (nToEdge + 1));
            Piece* targetPiece = b->GetSquare(targetSquare);

            // Blocked by friend
            if(targetPiece->GetC() == piece->GetC()){
                break; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});

            // If attacking place
            if(targetPiece->GetC() != piece->GetC() && targetPiece->GetC() != NULL_COLOUR){
                break; // Enemy encountered, end of direction
            }
        }
    }
}

// Pawn Movements
void GeneratePawnMovements(int Square, Piece* piece, Board* b){
    // Direction based on colour
    int StartIndex = (piece->GetC() == WHITE) ? 16 : 20;

    // TODO: Optimise!
    for (int directionI = StartIndex; directionI < StartIndex + 4; directionI += 1){
        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int offset = DirectionalOffsets[directionI];
        int targetSquare = Square + offset;
        Piece* targetPiece = b->GetSquare(targetSquare);

        if (abs(offset) == 8) {
            // Blocked by friend
            if(targetPiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});
        }
        else if (abs(offset) == 16) {
            // Blocked by friend
            if(targetPiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            int intermiateSquare = Square + (offset / 2);
            Piece* intermiatePiece = b->GetSquare(intermiateSquare);

            // Blocked by friend
            if(intermiatePiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            // Blocked by friend
            if(targetPiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});
        }
        else if (abs(offset) == 7 || abs(offset) == 9) {
            // Check for en-passent
            if(targetPiece->GetT() == NULL_TYPE){
                std::cout << "En-passent" << std::endl;
                // En-passent check
                int EnPassentSquare = targetSquare + ((piece->GetC() == WHITE) ? 8 : -8);

                if(targetPiece->moveCount == 1) {
                    std::cout << "Move count valid " << std::endl;
                }

                if (b->PlayedMoves.empty()) {
                    std::cout << "Played moves valid " << std::endl;
                    continue;
                }

                if(b->PlayedMoves.back().MovedPiece == targetPiece) {
                    std::cout << "Moved piece valid " << std::endl;
                }

                if(b->board[EnPassentSquare]->GetT() == PAWN) {
                    std::cout << "En-passent square valid " << std::endl;
                }

                // if(Square >= ((piece->GetC() == WHITE) ? 32 : 24) && Square <= ((piece->GetC() == WHITE) ? 39 : 31)){ // En-passent possible
                    if(targetPiece->moveCount == 1 && b->PlayedMoves.back().MovedPiece == targetPiece && b->board[EnPassentSquare]->GetT() == PAWN){
                        std::cout << "En-passent valid " << std::endl;
                        b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT(), {SPECIAL_EN_PASSENT, EnPassentSquare, b->board[EnPassentSquare], new Piece()}});
                    }
                // }

                // if(Square >= ((piece->GetC() == WHITE) ? 32 : 24) && Square <= ((piece->GetC() == WHITE) ? 39 : 31)){ // En-passent possible
                //     int EnPassentSquare = Square + ((piece->GetC() == WHITE) ? -8 : 8);
                //     if(b->board[EnPassentSquare]->moveCount == 1 && b->board[EnPassentSquare]->GetT() == PAWN){ // Valid? No
                //         // Ensure was the last performed move
                //         if(b->PlayedMoves.back().MovedPiece == b->board[EnPassentSquare]){
                //             b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT(), {SPECIAL_EN_PASSENT, EnPassentSquare, b->board[EnPassentSquare], new Piece()}});
                //         }
                //     }
                // }
                // continue;
            }
            else if(targetPiece->GetC() != piece->GetC()) {
                // rEGULAR dINGNAL mOVEMENT
                // all good
                b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});
            }
        }
    }
}

// Knight Movements
void GenerateKnightMovements(int Square, Piece* piece, Board* b){
    for(int directionI = 8; directionI < 16; directionI++){
        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int targetSquare = Square + DirectionalOffsets[directionI];
        Piece* targetPiece = b->GetSquare(targetSquare);

        // Blocked by friend
        if(targetPiece->GetC() == piece->GetC()){
            continue; // Wrong, so change direction
        }

        b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});
    }
}

// King Movements
void GenerateKingMovements(int Square, Piece* piece, Board* b){
    // Check all directions possible
    for (int directionI = 0; directionI < 8; directionI++){

        if (!SquaresToEdge[Square][directionI]) {
            continue;
        }

        int targetSquare = Square + DirectionalOffsets[directionI];
        Piece* targetPiece = b->GetSquare(targetSquare);

        // Blocked by friend
        if(targetPiece->GetC() == piece->GetC()){
            continue; // Wrong, so change direction
        }

        b->MoveList.push_back(Move{Square, targetSquare, targetPiece->GetT()});
    }

    // TODO: Castling
    if (piece->moveCount == 0) {
        // Offset of -2

        // Check if rook is there
        if (b->board[Square-4]->GetT() == ROOK && b->board[Square-4]->moveCount == 0) {
            // Check if path is clear
            if (b->board[Square-3]->GetT() == NULL_TYPE && b->board[Square-2]->GetT() == NULL_TYPE && b->board[Square-1]->GetT() == NULL_TYPE) {
                b->MoveList.push_back(Move{Square, Square-2, NULL_TYPE, {SPECIAL_CASTLING, Square-1, b->board[Square-4], new Piece()}});
            }
        }

        // Offset of +2

        // Check if rook is there
        if (b->board[Square+3]->GetT() == ROOK && b->board[Square+3]->moveCount == 0) {
            // Check if path is clear
            if (b->board[Square+1]->GetT() == NULL_TYPE && b->board[Square+2]->GetT()) {
                b->MoveList.push_back(Move{Square, Square+2, NULL_TYPE, {SPECIAL_CASTLING, Square+1, b->board[Square+3], new Piece()}});
            }
        }
    }
}