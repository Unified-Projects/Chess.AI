#include <chess/piece.h>
#include <chess/board.h>

// Piece Management Constants
    const int DirectionalOffsets[] = {
        8, -8,
        1, -1,
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

    for(int x = 0; x <= 7; x++){
        for(int y = 0; y <= 7; y++){
            // Generate directional data
            int North = 7 - y;
            int South = y;
            int East = 7 - x;
            int West = x;

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
                    if(x == 6 && y == 0){
                        std::cout << (int)(Index + DirectionalOffsets[i] / 8) << " " << (int)(Index + DirectionalOffsets[i] % 8) << std::endl;
                        std::cout << yChange << " " << xChange << std::endl;
                    }
                    continue;
                }

                knightStuff[i-8] = (pos <= 63 && pos >= 0 && (xChange == 2 && yChange == 1 || xChange == 1 && yChange == 2));
            }

            // Insert data
            int ComputedData[24] = {
                North,
                South,
                East,
                West,

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
            int targetSquare = Square + DirectionalOffsets[directionI] * (nToEdge + 1);
            Piece* targetPiece = b->GetSquare(targetSquare);

            // Blocked by friend
            if(targetPiece->GetC() == piece->GetC()){
                break; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare});

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

            b->MoveList.push_back(Move{Square, targetSquare});
        }
        else if (abs(offset) == 16) {
            // Blocked by friend
            if(targetPiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            int intermiateSquare = Square + offset + (offset / 2);
            Piece* intermiatePiece = b->GetSquare(intermiateSquare);

            // Blocked by friend
            if(intermiatePiece->GetT() != NULL_TYPE){
                continue; // Wrong, so change direction
            }

            b->MoveList.push_back(Move{Square, targetSquare});
        }
        else {
            // Check for en-passent
            if(targetPiece->GetT() == NULL_TYPE){
                // En-passent check
                // TODO: En-Passant Logic
                continue;
            }
            else if(targetPiece->GetC() != piece->GetC()) {
                // rEGULAR dINGNAL mOVEMENT
                // all good
                b->MoveList.push_back(Move{Square, targetSquare});
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

        b->MoveList.push_back(Move{Square, targetSquare});
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

        b->MoveList.push_back(Move{Square, targetSquare});
    }

    // TODO: Castling
}