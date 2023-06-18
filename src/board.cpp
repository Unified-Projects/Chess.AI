#include <chess/board.h>

// TEMPORARY INCLUDES
    #include <iostream>
        #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

// Static definition for mapping
std::map<char, Piece*> Board::pieceMapper = {
        {'r', new Rook(BLACK)}, {'R', new Rook(WHITE)},
        {'n', new Knight(BLACK)}, {'N', new Knight(WHITE)},
        {'p', new Pawn(BLACK)}, {'P', new Pawn(WHITE)},
        {'b', new Bishop(BLACK)}, {'B', new Bishop(WHITE)},
        {'q', new Queen(BLACK)}, {'Q', new Queen(WHITE)},
        {'k', new King(BLACK)}, {'K', new King(WHITE)},
    };
std::map<char, Type> Board::typeMapper = {
        {'r', ROOK}, {'R', ROOK},
        {'n', KNITGHT}, {'N', KNITGHT},
        {'p', PAWN}, {'P', PAWN},
        {'b', BISHOP}, {'B', BISHOP},
        {'q', QUEEN}, {'Q', QUEEN},
        {'k', KING}, {'K', KING},
    };

void Board::SetPiece(int X, int Y, Piece* p){
    // Load to the board
        // YX as in row then collumn
        // Yes we map 8 to 1 and 1 to 8 just as it's easier
    board[Y][X] = p;
    board[Y][X]->X = X + 1;
    board[Y][X]->Y = Y + 1;
    board[Y][X]->b = this; // So that pieces can acces the current board (Instead of global so that multiple boards can be active at once (Training AI))
}

void Board::InitBoard(std::string FEN) {
    /* FEN NOTATION
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

        Section 1: Board Layout
            rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

            '/' Means new row, caps are white and the numbers are the spaces before the
                next piece

        Section 2: Active Colour
            The next turn, or who is to play first

        Section 3: Castling availability
            Folling Caps from section 1
                QK defines that the Rooks on both the queens side and the kings side
                    are playable.

        Section 4: Moves
            Will be a list of moves that have been played ('-' means no moves)

        Section 5: Half Clock
            Number of moves since the last capture or pawn advance
            Also used to draw a match

        Section 6: Fullmove number
            Number of moves that have been played (Incremented after black's move)
    */

    // TODO: Error Checking on the inputted FEN

    // Used for iteration and a local position vector for setting pieces
    const char* FEN_STR = FEN.c_str();
    int LocalX = 0;
    int LocalY = 0;

    // Iterate over the FEN inputted notation for parsing
    for(; *FEN_STR; FEN_STR++){

        /*
            NOTE: We are not following the extra sections of the set notation
                so as a result we break as soon as the section ends.
                We may implement later on, but majority / all games
                will start from default.
        */

        // Check if it is a digit
        if (*FEN_STR >= '1' && *FEN_STR <= '8') {
            for (int i = 0; i < *FEN_STR - 48; i++) {
                // board[LocalY][LocalX + i] = new Piece();
                SetPiece(LocalX + i, 7 - LocalY, new Piece());
            }
            LocalX += *FEN_STR - 48;
            continue;
        }

        // New board row
        if (*FEN_STR == '/') {
            LocalY++;
            LocalX = 0;
            continue;
        }

        // Section end so we end
        if (*FEN_STR == ' ') {
            break;
        }

        // Piece assignment according to map
        Piece* newPiece = pieceMapper[*FEN_STR]->Clone();
        SetPiece(LocalX, 7 - LocalY, newPiece);

        if (newPiece->t == KING){
            if (newPiece->c == WHITE){
                WhiteKing = newPiece;
            }
            else{
                BlackKing = newPiece;
            }
        }

        // Move horizonatally
        LocalX++;
    }

    // See if either in check
    UpdateCheck();

    return;
}

void Board::UpdateCheck(){
    // Im going to do a in-efficient check checker because im lazy and want the functionality now
        // The better option would be to check all move optiosn out of the kings position
        // and see if they are a opposing piece capable of the move (Dont search further than it if it blocks other)

    bool PrevW = WhiteCheck;
    bool PrevB = BlackCheck;

    WhiteCheck = false;
    BlackCheck = false;

    for(int x = 1; x <= 8; x++){
        for (int y = 1; y <= 8; y++){
            Piece* targetPiece = GetPieceAtPosition(x, y);
            if(targetPiece->GetC() == NULE){
                continue;
            }

            // Piece not moved or Attacking team-mate or Attacking team-mate
            if (!(targetPiece->X == WhiteKing->X && targetPiece->Y == WhiteKing->Y) && targetPiece->GetC() != WHITE){
                WhiteCheck = targetPiece->isValidMove(WhiteKing->X, WhiteKing->Y); // WHITE
            }
            // Piece not moved or Attacking team-mate or Attacking team-mate
            if (!(targetPiece->X == BlackKing->X && targetPiece->Y == BlackKing->Y) && targetPiece->GetC() != BLACK){
                BlackCheck = targetPiece->isValidMove(BlackKing->X, BlackKing->Y); // BLACK
            }

            if(WhiteCheck || BlackCheck){
                break;
            }
        }
        if(WhiteCheck || BlackCheck){ // May lead to issue by seeing if only one is in check, but standard should be this
            break;
        }
    }

    return;
}

void Board::LogBoard(){
    // Iterate both over rows and columns
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {

            // std::cout << '(' << j+1 << ',' << i+1 << ')';

            // continue;

            // Get the current pieces colour and type
            Type t = board[i][j]->t;
            Colour c = board[i][j]->c;

            // Create a store point for the character
            char type = ' ';

            // Reverse the map to get the character linking to the specified type
            std::for_each(typeMapper.begin(), typeMapper.end(),
                        /*Reference points for returning the values out of the embed*/[&t, &type](const std::pair<char, Type> &p)
                        {
                            // Simple compare until found
                            if (p.second == t)
                            {
                                type = p.first;
                            }
                        });

            // Colour formatting dependent of the pieces colour
            const char* ColorMod = "";
            if (c == WHITE){
                ColorMod = "\033[1m";
            }
            else{
                ColorMod = "\033[1;30m";
            }

            // Log piece
            std::cout << ColorMod << type << "\033[0;m ";
        }
        // New row so new line
        std::cout << std::endl;
    }

    return;
}

Piece* Board::GetPieceAtPosition(int X, int Y) {
    // Note as our list goes 0-7 from top to bottom
    //      We need to map the positions to standards
    return board[Y-1][X-1];
}

// Standard using letters for the X-Axis
Piece* Board::GetPieceAtPosition(char X, int Y) {
    // Note as our list goes 0-7 from top to bottom
    //      We need to map the positions to standards
    return board[8-Y][X - 65]; // Subtract 'A' to perfom mapping
}

bool Board::MovePiece(int startX, int startY, int targetX, int targetY) {
    // First find the piece that we want to move
    Piece* targetPiece = GetPieceAtPosition(startX, startY);
    Piece* endPiece = GetPieceAtPosition(targetX, targetY);

    // Record the type
    Type t = targetPiece->t;

    // Check if the target location is within the board
    if (targetX < 1 || targetX > 8 || targetY < 1 || targetY > 8) {
        return false;
    }

    // Piece not moved or Attacking team-mate or Attacking team-mate
    if ((startX == targetX && startY == targetY) || endPiece->GetC() == targetPiece->GetC()){
        return false;
    }

    MoveExtra Change = {};

    // We want to check if the provided swap is a valid move
    bool validMove = targetPiece->isValidMove(targetX, targetY, &Change);
    if (!validMove) { // Return as the move impossible (Handler built on other end)
        return false;
    }

    // Extras / Special Moves
    if(Change.type){
        if (Change.type == 1){
            SetPiece(Change.x-1, Change.y-1, new Piece());
        }
    }

    // See if the pawn changed piece
    if(t == PAWN && targetPiece->t != PAWN){
        // TODO: Change depending on new type
        // Piece* Changed = ((Queen*)targetPiece)->Clone();
        Piece* Changed = new Queen(*((Queen*)targetPiece));

        // Store Pawn for move undo
        Piece* Old = targetPiece;
        Old->t = PAWN;

        // Replace Pawn
        targetPiece = Changed;

        // Store Move with old piece
        PlayedMoves.push_back(MoveCache{Old, endPiece, startX, startY, targetX, targetY, Change});
    }
    else{
        // Store Move normally
        PlayedMoves.push_back(MoveCache{targetPiece, endPiece, startX, startY, targetX, targetY, Change});
    }

    // If so we want to move the piece, and then delete the piece that was already there
    SetPiece(targetX-1, targetY-1, targetPiece);

    // Fill in gap made
    SetPiece(startX-1, startY-1, new Piece());

    // TODO: UPDATE GAME STATS, IF SPECIAL MOVES ARE ALLOWED, LOOK FOR CHECKMATE

    // Used for seeing if the check condition does not change
    bool PrevW = WhiteCheck;
    bool PrevB = BlackCheck;

    // Check Check
    UpdateCheck();

    // If the check condition does not change then the move is invalid
    if((WhiteCheck && PrevW && targetPiece->c == WHITE) || (BlackCheck && PrevB && targetPiece->c == BLACK)){
        UndoMove();
        return false;
    }

    // Increment move count
    targetPiece->moveCount++;

    return true;
}

void Board::UndoMove(){
    // Simple check to see if moves have been played
    if (PlayedMoves.empty()){
        return; // No move to undo
    }

    // Lets to a simple usage
    MoveCache Move = PlayedMoves.back();
    PlayedMoves.pop_back();
    SetPiece(Move.StartX-1, Move.StartY-1, Move.MovedPiece);
    SetPiece(Move.EndX-1, Move.EndY-1, Move.TargetPiece);

    // Extras / Special Move Undoing
    if(Move.Extra.type){
        if (Move.Extra.type == 1){
            SetPiece(Move.Extra.x-1, Move.Extra.y-1, Move.Extra.change);
        }
    }

    // Decrease move count
    Move.MovedPiece->moveCount--;

    return;
}