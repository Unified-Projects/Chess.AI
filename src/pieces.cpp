#include <chess/piece.h>
#include <chess/board.h>

// Piece Management Constants
    const int DirectionalOffsets[] = {
        8, -8,
        1, -1,
        7, -7,
        9, -9
    };
//

// To run at the begining
void PrecomputeEdges(){
    static bool Runnable;

    if(!Runnable){
        return;
    }

    for(int x = 0; x < 7; x++){
        for(int y = 0; y < 7; y++){
            // Generate directional data
            int North = 7 - y;
            int South = y;
            int East = 7 - x;
            int West = x;

            // Calculate index
            int Index = (y * 8) + x;

            // Insert data
            SquaresToEdge[Index] = {
                North,
                South,
                East,
                West,

                // Extras
                std::min(North, West),
                std::min(South, East),
                std::min(North, East),
                std::min(South, West),
            };
        }
    }

    // Restore
    Runnable = true;
    return;
}

// Used for queens, bishops, rooks
void GenerateSlidingMoves(int Square, Piece* piece, Board* b){
    // Compatible directions
    int StartIndex = (piece->GetCapabilites() & SLIDE_DIAGONAL) ? 4 : 0;
    int EndIndex = (piece->GetCapabilites() & SLIDE_HORIZONTAL) ? 4 : 8;

    // Check all directions possible
    for (int directionI = StartIndex; directionI < EndIndex; directionI++){
        for(int nToEdge = 0; nToEdge < SquaresToEdge[Square][directionI]; nToEdge++){
            int targetSquare = Square + DirectionalOffsets[directionI] * (nToEdge + 1);
            Piece* targetPiece = b->GetSquare(targetSquare);

            // Blocked by friend
            if(targetPiece->GetC() == piece->GetC()){
                break; // Wrong, so change direction
            }

            b->MoveList.push_back(new Move{Square, targetSquare});

            // If attacking place
            if(targetPiece->GetC() == !piece->GetC()){
                break; // Enemy encountered, end of direction
            }
        }
    }
}