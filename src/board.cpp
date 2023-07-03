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
std::map<Type, char> Board::typeMapper = {
        {ROOK, 'r'}, {ROOK, 'R'},
        {KNIGHT, 'n'}, {KNIGHT, 'N'},
        {PAWN, 'p'}, {PAWN, 'P'},
        {BISHOP, 'b'}, {BISHOP, 'B'},
        {QUEEN, 'q'}, {QUEEN, 'Q'},
        {KING, 'k'}, {KING, 'K'},
        {NULL_TYPE, ' '}
    };

// void Board::SetPiece(int X, int Y, Piece* p){
//     // Load to the board
//         // YX as in row then collumn
//         // Yes we map 8 to 1 and 1 to 8 just as it's easier
//     board[Y][X] = p;
//     board[Y][X]->X = X + 1;
//     board[Y][X]->Y = Y + 1;
//     board[Y][X]->b = this; // So that pieces can acces the current board (Instead of global so that multiple boards can be active at once (Training AI))
// }

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
    // const char* FEN_STR = FEN.c_str();
    // int LocalX = 0;
    // int LocalY = 0;

    // // Iterate over the FEN inputted notation for parsing
    // for(; *FEN_STR; FEN_STR++){

    //     /*
    //         NOTE: We are not following the extra sections of the set notation
    //             so as a result we break as soon as the section ends.
    //             We may implement later on, but majority / all games
    //             will start from default.
    //     */

    //     // Check if it is a digit
    //     if (*FEN_STR >= '1' && *FEN_STR <= '8') {
    //         for (int i = 0; i < *FEN_STR - 48; i++) {
    //             // board[LocalY][LocalX + i] = new Piece();
    //             SetPiece(LocalX + i, 7 - LocalY, new Piece());
    //         }
    //         LocalX += *FEN_STR - 48;
    //         continue;
    //     }

    //     // New board row
    //     if (*FEN_STR == '/') {
    //         LocalY++;
    //         LocalX = 0;
    //         continue;
    //     }

    //     // Section end so we end
    //     if (*FEN_STR == ' ') {
    //         break;
    //     }

    //     // Piece assignment according to map
    //     Piece* newPiece = pieceMapper[*FEN_STR]->Clone();
    //     SetPiece(LocalX, 7 - LocalY, newPiece);

    //     if (newPiece->t == KING){
    //         if (newPiece->c == WHITE){
    //             WhiteKing = newPiece;
    //         }
    //         else{
    //             BlackKing = newPiece;
    //         }
    //     }

    //     if (newPiece->c == WHITE){
    //         WhitePieces.push_back(newPiece);
    //     }
    //     else{
    //         BlackPieces.push_back(newPiece);
    //     }

    //     // Move horizonatally
    //     LocalX++;
    // }

    // // See if either in check
    // UpdateCheck();

    return;
}

// bool Board::UpdateCheck() {
//     Check = false;
//     CheckedColour = NULL_COLOUR;

//     // Find the opponent's king position
//     int whiteKingX = WhiteKing->X;
//     int whiteKingY = WhiteKing->Y;
//     int blackKingX = BlackKing->X;
//     int blackKingY = BlackKing->Y;

//     // See if Black is in check
//     for (Piece* targetPiece : WhitePieces) {
//         // Check if the piece can move to the opponent's king
//         if (targetPiece->isValidMove(blackKingX, blackKingY)) {
//             Check = true;
//             CheckedColour = BLACK;
//             return Check;
//         }
//     }

//     // See if White is in check
//     for (Piece* targetPiece : BlackPieces) {
//         // Check if the piece can move to the opponent's king
//         if (targetPiece->isValidMove(whiteKingX, whiteKingY)) {
//             Check = true;
//             CheckedColour = WHITE;
//             return Check;
//         }
//     }

//     return Check;
// }

// bool Board::UpdateCheckmate() {
//     if (!UpdateCheck()) {
//         return false;
//     }

//     Mate = false;

//     // Generate moves for the pieces of the color under check
//     std::list<Piece*> pieces = (CheckedColour == WHITE) ? WhitePieces : BlackPieces;

//     for (Piece* p : pieces){
//         for (std::pair<int, int> move : MoveGen(p->t)){
//             // Play the move
//             bool validMove = MovePiece(p->X, p->Y, p->X + move.first, p->Y + move.second, true);

//             if (validMove && !UpdateCheck()) {
//                 // Move eliminates the check, so it's not checkmate
//                 UndoMove();
//                 return false;
//             }

//             // Undo the move if it was valid
//             if (validMove) {
//                 UndoMove();
//             }
//         }
//     }

//     Mate = true;
//     return true;
// }

// void Board::LogBoard(){
//     // Iterate both over rows and columns
//     for (int i = 7; i >= 0; i--) {
//         for (int j = 0; j < 8; j++) {

//             // std::cout << '(' << j+1 << ',' << i+1 << ')';

//             // continue;

//             // Get the current pieces colour and type
//             Type t = board[i][j]->t;
//             Colour c = board[i][j]->c;

//             // Create a store point for the character
//             char type = this->typeMapper[t];

//             // Colour formatting dependent of the pieces colour
//             const char* ColorMod = "";
//             if (c == WHITE){
//                 ColorMod = "\033[1m";
//             }
//             else{
//                 ColorMod = "\033[1;30m";
//             }

//             // Log piece
//             std::cout << ColorMod << type << "\033[0;m ";
//         }
//         // New row so new line
//         std::cout << std::endl;
//     }

//     return;
// }

// std::string Board::ConvertToFen() { // TODO: EFFICIENCY CHECK
//     std::string fen = "";
//     int nullCounter = 0;
//     for (int y=8; y>=1; y--) {
//         for (int x=1; x<=8; x++) {
//             // Get piece at position
//             Type type = GetPieceAtPosition(x, y)->GetT();
//             Colour colour = GetPieceAtPosition(x, y)->GetC();

//             if (type == NULL_TYPE) {
//                 nullCounter++;
//                 continue;
//             }
//             if (nullCounter) {
//                 fen += std::to_string(nullCounter);
//                 nullCounter = 0;
//             }
//             char t = Board::typeMapper[type] + ((colour == WHITE) ? ('A' - 'a') : 0);
//             fen += t;

//             // TODO: Other FEN Sections Needed!
//         }
//         if (nullCounter) {
//             fen += std::to_string(nullCounter);
//             nullCounter = 0;
//         }
//         if (y != 1) {
//             fen += "/";
//         }
//     }

//     return fen;
// }

// bool Board::MovePiece(int startX, int startY, int targetX, int targetY, bool ignoreCheck) {
//     // TODO: IMPLEMENT A END-GAME HANDLER!!!! AND FOR TESTER
//     // if (Mate){
//     //     // LogBoard();
//     //     return false;
//     // }

//     // Check if the target location is within the board
//     if (startX < 1 || startX > 8 || startY < 1 || startY > 8) {
//         return false;
//     }

//     // Check if the target location is within the board
//     if (targetX < 1 || targetX > 8 || targetY < 1 || targetY > 8) {
//         return false;
//     }

//     // First find the piece that we want to move
//     Piece* targetPiece = GetPieceAtPosition(startX, startY);
//     Piece* endPiece = GetPieceAtPosition(targetX, targetY);

//     // Record the type
//     Type t = targetPiece->t;

//     // Piece not moved or Attacking team-mate or Attacking team-mate
//     if (endPiece->GetC() == targetPiece->GetC()){
//         return false;
//     }

//     MoveExtra Change = {};

//     // We want to check if the provided swap is a valid move
//     bool validMove = targetPiece->isValidMove(targetX, targetY, &Change);
//     if (!validMove) { // Return as the move impossible (Handler built on other end)
//         return false;
//     }

//     // Extras / Special Moves
//     if(Change.type){
//         if (Change.type == 1){
//             SetPiece(Change.x-1, Change.y-1, new Piece());
//         }
//     }

//     // See if the pawn changed piece
//     // if(t == PAWN && targetPiece->t != PAWN){
//     //     // TODO: Change depending on new type
//     //     // Piece* Changed = ((Queen*)targetPiece)->Clone();
//     //     Piece* Changed = new Queen(*((Queen*)targetPiece));

//     //     // Store Pawn for move undo
//     //     Piece* Old = targetPiece;
//     //     Old->t = PAWN;

//     //     // Replace Pawn
//     //     targetPiece = Changed;

//     //     // Store Move with old piece
//     //     PlayedMoves.push_back(MoveCache{Old, endPiece, startX, startY, targetX, targetY, Change});
//     // }
//     // else{
//         // Store Move normally
//         PlayedMoves.push_back(MoveCache{targetPiece, endPiece, startX, startY, targetX, targetY, Change});
//     // }

//     if(endPiece->t != NULL_TYPE){
//         std::list<Piece*>* pieces = (endPiece->c == WHITE) ? &WhitePieces : &BlackPieces;
//         pieces->remove(endPiece);
//     }

//     // If so we want to move the piece, and then delete the piece that was already there
//     SetPiece(targetX-1, targetY-1, targetPiece);

//     // Fill in gap made
//     SetPiece(startX-1, startY-1, new Piece());

//     // TODO: UPDATE GAME STATS, IF SPECIAL MOVES ARE ALLOWED, LOOK FOR CHECKMATE

//     // Used for seeing if the check condition does not change
//     bool Prev = Check;
//     Colour PrevC = CheckedColour;

//     // Check Checks
//     UpdateCheck();

//     // Increment move count
//     targetPiece->moveCount++;

//     // If the check condition does not change then the move is invalid or we go into check
//     if(((Check && Prev && PrevC == CheckedColour) || (Check && CheckedColour == targetPiece->c)) && !ignoreCheck){
//         UndoMove();
//         return false;
//     }

//     return true;
// }

// void Board::UndoMove(){
//     // Simple check to see if moves have been played
//     if (PlayedMoves.empty()){
//         return; // No move to undo
//     }

//     // Lets to a simple usage
//     MoveCache Move = PlayedMoves.back();
//     PlayedMoves.pop_back();
//     SetPiece(Move.StartX-1, Move.StartY-1, Move.MovedPiece);
//     SetPiece(Move.EndX-1, Move.EndY-1, Move.TargetPiece);

//     // Extras / Special Move Undoing
//     if(Move.Extra.type){
//         if (Move.Extra.type == 1){
//             SetPiece(Move.Extra.x-1, Move.Extra.y-1, Move.Extra.change);
//         }
//     }

//     // Load the piece back if taken
//     if(Move.TargetPiece->t != NULL_TYPE){
//         std::list<Piece*>* pieces = (Move.TargetPiece->c == WHITE) ? &WhitePieces : &BlackPieces;
//         pieces->push_back(Move.TargetPiece);
//     }

//     // Resort Check
//     UpdateCheck();

//     // Decrease move count
//     Move.MovedPiece->moveCount--;

//     return;
// }

std::list<Move*> Board::GenerateMoves(){
    MoveList = {};

    // Loop over all squares
    for (int Square = 0; Square < 64; Square++){
        Piece* p = board[Square]; // Select Piece

        // Ensure its the pieces turn to move
        if(p->c != CurrentMove){ 
            continue;
        }

        // Sliding piece
        if(p->MovingCapabilites & SLIDE){
            // Generate all possible sliding moves for that piece
            GenerateSlidingMoves(Square, p, this);
        }
        else if(p->MovingCapabilites & PAWN_MOVMENT){
            GeneratePawnMovements(Square, p, this);
        }
        else if(p->MovingCapabilites & KNIGHT_MOVMENT){
            GenerateKnightMovements(Square, p, this);
        }
        else if(p->MovingCapabilites & KING_MOVMENT){
            GenerateKingMovements(Square, p, this);
        }
    }

    return MoveList;
}