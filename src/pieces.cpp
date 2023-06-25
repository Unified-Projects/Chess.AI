#include <chess/piece.h>
#include <chess/board.h>

// TEMPORARY INCLUDES
    #include <iostream>
    #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

// TODO: For the MoveExtra when we need to use it we need to see if it exits!!

// Partial Tests
bool SlideTest(int xDir, int yDir, int Distance, Piece* TargetPiece, Board* b){
    // Check over the direction
    for(int i = 1; i < Distance; i++){
        if(b->GetPieceAtPosition(TargetPiece->X + (xDir*i), TargetPiece->Y + (yDir*i))->GetT() != NULL_TYPE){
            return false;
        }
    }

    return true;
}

bool Pawn::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    /*
        Movement:
            Forward 1 (2 if first move)
            Diagonal 1 if enemy piece present

        Speical: En Passent
            If it's first move and becomes adjasent to another pawn,
            that pawn can take piece by going diagonal into a empty space.
    */

    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Check if piece can move forward
    // Check if the target is in the same column
    if (targetX == X && targetPiece->GetT() == NULL_TYPE) {
        // Calculate the pieces forward (Depending on colour)
        int diff = (targetY - Y) * ((c == WHITE) ? 1 : -1);

        // Make sure it's valid for the move
        if (diff <= ((moveCount == 0) ? 2 : 1) && diff > 0){

            if (moveCount == 0 && diff == 2) {
                // Check if there is a piece in the way
                if (b->GetPieceAtPosition(X, Y + ((c == WHITE) ? 1 : -1))->GetT() != NULL_TYPE) {
                    return false;
                }
            }

            if((c == WHITE && targetY == 8) || (c == BLACK && targetY == 1)){
                //TODO: Capability to choose the piece to become
                //TODO: MOVE TO USE Extra Not change type
                t = QUEEN;
            }
            return true;
        }
    }
    // Check for taking
    else { //TODO: Implement a taking system to keep track of whats been taken for all pieces
        // Check if the target is in neighbouring column
        if (abs(targetX - X) == 1) {
            // Check if the target is in the next row
            if (targetY == Y + ((c == WHITE) ? 1 : -1)) {
                // Check if enemy piece is present at target
                if (targetPiece->GetT() != NULL_TYPE) {
                    return true;
                }

                // En Passent
                /* Requirements
                    The capturing pawn must have already moved exactly three ranks to perform this move.
                    The captured pawn must have moved two squares in one move, landing right next to the capturing pawn. (First move)
                    The en passant capture must be performed on the turn immediately after the pawn being captured moves.
                */
                Piece* EnPassentPiece = b->GetPieceAtPosition(targetX, targetY + ((c) ? -1 : 1));

                // Check piece is valid
                if(!EnPassentPiece || EnPassentPiece->GetT() != PAWN){
                    return false;
                }

                // See if piece has only just moved and moved 2
                if(b->PlayedMoves.back().MovedPiece == EnPassentPiece){
                    if (((EnPassentPiece->GetC() == BLACK && EnPassentPiece->Y == 5) || (EnPassentPiece->GetC() == WHITE && EnPassentPiece->Y == 4)) && EnPassentPiece->moveCount == 1 && EnPassentPiece->GetC() != c){
                        if(!Extra){
                            return true; // Still a valid move
                        }

                        // En-passent possible
                        (*Extra) = {1 /*En Passent*/, EnPassentPiece->X, EnPassentPiece->Y, EnPassentPiece};
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool Knight::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Basic shape is the only requirment
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    if((dx == 1 && dy == 2) || (dx == 2 && dy == 1)){
        return true;
    }

    return false;
}

bool Rook::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Direction
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);
    int DirBlock = 0;

    if(dx && dy){
        return false;
    }

    if(0 == dx && 0 != dy){
        // Now follow in direction of y +- depending on dy
        DirBlock = 1;
    }
    else if(0 != dx && 0 == dy){
        // Now follow in direction of x +- depending on dx
        DirBlock = 2;
    }

    if(!SlideTest(dirx * (DirBlock != 1), diry * (DirBlock != 2), (dx) ? dx : dy, this, b)){
        return false;
    }

    return true;
}

bool Bishop::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Direction
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);

    // Check if move is diagonal
    if(dx != dy) {
        return false;
    }

    if(!SlideTest(dirx, diry, dx, this, b)){
        return false;
    }
    return true;
}

bool Queen::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Direction
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = abs(targetX - X);
    int dy = abs(targetY - Y);
    int DirBlock = 0;

    if(0 == dx && 0 != dy){
        // Now follow in direction of y +- depending on dy
        DirBlock = 1;
    }
    else if(0 != dx && 0 == dy){
        // Now follow in direction of x +- depending on dx
        DirBlock = 2;
    }
    else if(dx != dy) {
        return false;
    }
    
    if(!SlideTest(dirx * (DirBlock != 1), diry * (DirBlock != 2), (dx) ? dx : dy, this, b)){
        return false;
    }

    return true;
}

bool King::isValidMove(int targetX, int targetY, MoveExtra* Extra) {
    // Get the piece at position we aim for
    Piece* targetPiece = b->GetPieceAtPosition(targetX, targetY);

    // Direction
    int dirx = (targetX - X > 0) ? 1 : -1;
    int diry = (targetY - Y > 0) ? 1 : -1;
    int dx = targetX - X;
    int dy = targetY - Y;

    // Castling
    /*
        Direction dependent
        The king or castle cant move before
        Must not be in check, or have the move go through being in check
        No pieces in between
    */
    // TODO: see if currently in check
        // Perfom the partion (going through check), then check check
        // then undo move if in check and return that the move is invalid
    // TODO: Fix the checking along the board (Distance is to the castle not to where the king goes!)
    // if(abs(dx) == 2 && dy == 0 && moveCount == 0){
    //     int dist = 0;
    //     if(dx < 0){
    //         dist = -4;
    //     }
    //     else{
    //         dist = 3;
    //     }

    //     // Check the castle has not moved
    //     Piece* castle = b->GetPieceAtPosition(X + dist, Y);
    //     if(castle->moveCount != 0 && castle->GetT() == ROOK && castle->GetC() == c){
    //         return false;
    //     }

    //     // Now follow in direction of x +- depending on dx
    //     for (int x = dirx; x != dist; x += dirx){
    //         if (b->GetPieceAtPosition(X + x, Y)->GetT() != NULL_TYPE){ // Piece present and there cant be
    //             return false;
    //         }

    //         // See if we move into check
    //         bool Valid = b->MovePiece(X, Y, X + x, Y);

    //         if(Valid){
    //             b->UndoMove();
    //         }
    //     }

    //     // MOVE CASTLES BIG ISSUE UNDOOOOOO // TODO: MOVEEXTRAS
    //     if(dx < 0){
    //         b->MovePiece(1, Y, 4, Y);
    //     }
    //     else{
    //         b->MovePiece(8, Y, 6, Y);
    //     }

    //     return true;
    // }

    if (abs(dx) == 1 && abs(dy) == 1 ||  abs(dx) == 0 && abs(dy) == 1 || abs(dx) == 1 && abs(dy) == 0){
        return true;
    }

    return false;
}
