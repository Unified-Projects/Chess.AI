#include <chess/board.h>
#include <cstring>

// TEMPORARY INCLUDES
    #include <iostream>
        #define UNIMPLEMENTED std::cout << "Unimplemented " << __FILE__ << ":" << __LINE__ << std::endl;
//

// Static definition for mapping
std::map<char, Type> Board::pieceMapper = {
        {'r', ROOK}, {'R', ROOK},
        {'n', KNIGHT}, {'N', KNIGHT},
        {'p', PAWN}, {'P', PAWN},
        {'b', BISHOP}, {'B', BISHOP},
        {'q', QUEEN}, {'Q', QUEEN},
        {'k', KING}, {'K', KING},
    };

std::map<Type, char> Board::typeMapper = {
        {ROOK, 'r'}, {ROOK, 'R'},
        {KNIGHT, 'n'}, {KNIGHT, 'N'},
        {PAWN, 'p'}, {PAWN, 'P'},
        {BISHOP, 'b'}, {BISHOP, 'B'},
        {QUEEN, 'q'}, {QUEEN, 'Q'},
        {KING, 'k'}, {KING, 'K'},
        {NULL_TYPE, '.'} // Looks easier to understand the board
    };

Board::Board() {
    // Check management
    Check = false;
    Stale = false;
    Mate = false;

    // Color
    CheckedColour = NULL_COLOUR;
    CurrentMove = WHITE;

    // King savings
    WhiteKing = -1;
    BlackKing = -1;

    MoveList = {};
    PreviousGeneration = NULL_COLOUR; // Caching

    // Generate a board
    board = new short[64];
    std::memset(board, 0, 128);

    return;
}
int c = 1;
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

        Section 4: En Passent
            If En Passent is possible and where ('-' means no moves)

        Section 5: Half Clock
            Number of moves since the last capture or pawn advance
            Also used to draw a match

        Section 6: Fullmove number
            Number of moves that have been played (Incremented after black's move)
    */

    // TODO: Error Checking on the inputted FEN

    // Re-init all
    // Check management
    Check = false;
    Stale = false;
    Mate = false;

    // Color
    CheckedColour = NULL_COLOUR;
    CurrentMove = WHITE;

    // King savings
    WhiteKing = -1;
    BlackKing = -1;

    // Piece
    WhitePieces = {};
    BlackPieces = {};

    MoveList = {};
    PreviousGeneration = NULL_COLOUR; // Caching

    // Used for iteration and a local position vector for setting pieces
    const char* FEN_STR = FEN.c_str();
    int LocalRank = 7;
    int LocalFile = 0;
    int Section = 0;

    // Easier Visuals
    enum Sections{
        FEN_BOARD = 0x00,
        FEN_ACTIVE_COLOUR,
        FEN_CASTLING_RIGHTS,
        FEN_EN_PASSANT,
        FEN_HALF_MOVES,
        FEN_FULL_MOVES
    };

    // TODO: Fix loading of board, currently backwards
    // // Iterate over the FEN inputted notation for parsing
    for(; *FEN_STR; FEN_STR++){

        /*
            NOTE: We are not following the extra sections of the set notation
                so as a result we break as soon as the section ends.
                We may implement later on, but majority / all games
                will start from default.
        */

        // Use if statements as we use continue / break and switch would break them
        if(Section == FEN_BOARD){ // This section loads the board info
            { // Extra indents
                // Check if it is a digit
                if (*FEN_STR >= '1' && *FEN_STR <= '8') {
                    for (int i = 0; i < *FEN_STR - 48; i++) {
                        SET_NULL(board[(LocalRank * 8) + LocalFile + i]);
                    }

                    LocalFile += *FEN_STR - 48;
                    continue;
                }

                // New board row
                if (*FEN_STR == '/') {
                    LocalFile = 0;
                    LocalRank--;
                    continue;
                }

                // Section end so we stop loading board information
                if (*FEN_STR == ' ') { // Section Ends are specific to the section
                    Section++;
                    continue;
                }

                // Sqaure info
                int Square = (LocalRank * 8) + LocalFile;

                // Colours
                Colour colour;

                // Capital
                if(*FEN_STR < 'Z'){
                    colour = WHITE;
                }
                // Lowercase
                else{
                    colour = BLACK;
                }

                // Find the type
                switch (pieceMapper[*FEN_STR]){
                    case PAWN:
                    {
                        SET_PAWN(board[Square], colour);
                        break;
                    }

                    case ROOK:
                    {
                        SET_ROOK(board[Square], colour);
                        break;
                    }

                    case KNIGHT:
                    {
                        SET_KNIGHT(board[Square], colour);
                        break;
                    }

                    case BISHOP:
                    {
                        SET_BISHOP(board[Square], colour);
                        break;
                    }

                    case QUEEN:
                    {
                        SET_QUEEN(board[Square], colour);
                        break;
                    }

                    case KING:
                    {
                        SET_KING(board[Square], colour);

                        if (colour == WHITE){
                            WhiteKing = Square;
                        }
                        else{
                            BlackKing = Square;
                        }
                        break;
                    }

                    default:
                        std::cerr << "ERROR WITH FEN" << std::endl;
                        break;
                }

                if (colour == WHITE){
                    WhitePieces.push_back(Square);
                }
                else{
                    BlackPieces.push_back(Square);
                }

                // Move horizonatally
                LocalFile++;
            }
        }
        else if(Section == FEN_ACTIVE_COLOUR){
            if(*FEN_STR == 'b' || *FEN_STR == 'B'){
                // Blacks turn
                CurrentMove = BLACK;
            }
            else if(*FEN_STR == 'w' || *FEN_STR == 'W'){
                // Whites turn
                CurrentMove = WHITE;
            }
            else{ // Section Ends
                Section++;
                continue;
            }
        }
        else if(Section == FEN_CASTLING_RIGHTS){
            // TODO: Store if castle and then allow castling if board allow it
            // Note: This is not a required feature as we will mostly work with standard boards! (Except testing)

            if(*FEN_STR == 'q' || *FEN_STR == 'B'){
                // Black Queen Castle
            }
            else if(*FEN_STR == 'k'){
                // Black King Castle
            }
            else if(*FEN_STR == 'Q'){
                // White Queen Castle
            }
            else if(*FEN_STR == 'K'){
                // White King Castle
            }
            else if(*FEN_STR == '-'){
                // None
            }
            else{ // Section Ends
                Section++;
                continue;
            }
        }
        else if(Section == FEN_EN_PASSANT){
            if(*FEN_STR == ' ') { // Section Ends
                Section++;
                continue;
            }

            if(*FEN_STR == '-'){
                // No targets
            }
            else{
                // This and next char stuff to outline target square
            }
        }
        else if(Section == FEN_HALF_MOVES){
            if(*FEN_STR == ' ') { // Section Ends
                Section++;
                continue;
            }

            // Some sort of character processing to get the number
        }
        else if(Section == FEN_FULL_MOVES){
            if(*FEN_STR == ' ') { // Section Ends
                Section++;
                continue;
            }

            // Some sort of character processing to get the number
        }
    }

    // Current Move
    CurrentMove = WHITE;

    c = 1;

    // See if either in check
    // UpdateCheck();

    return;
}

void Board::LogBoard(){
    // Iterate both over rows and columns
    for (int i = 7; i >=0; i--) {
        for (int j = 0; j < 8; j++) {

            // std::cout << '(' << j+1 << ',' << i+1 << ')';

            // continue;

            // Get the current pieces colour and type
            Type t = GetType(board[(i*8) + j]);
            Colour c = GetColour(board[(i*8) + j]);

            // Create a store point for the character
            char type = this->typeMapper[t];

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

std::string Board::ConvertToFen() { // TODO: EFFICIENCY CHECK
    std::string fen = "";
    int nullCounter = 0;
    for (int y=7; y>=0; y--) {
        for (int x=0; x<=7; x++) {
            // Get piece at position
            Type t = GetType(board[(y*8) + x]);
            Colour colour = GetColour(board[(y*8) + x]);

            if (t == NULL_TYPE) {
                nullCounter++;
                continue;
            }
            if (nullCounter) {
                fen += std::to_string(nullCounter);
                nullCounter = 0;
            }
            char type = Board::typeMapper[t] + ((colour == WHITE) ? ('A' - 'a') : 0);
            fen += type;

            // TODO: Other FEN Sections Needed!
        }
        if (nullCounter) {
            fen += std::to_string(nullCounter);
            nullCounter = 0;
        }
        if (y != 0) {
            fen += "/";
        }
    }

    return fen;
}

std::list<Move> Board::GenerateMoves(){
    if (CurrentMove == PreviousGeneration && !RegenNeeded){
        return MoveList;
    }

    MoveList = {};

    // If we are in double check we can only move king
    if(CurrentMove == CheckedColour && CheckCount > 1){
        GenerateKingMovements((CurrentMove == WHITE) ? WhiteKing : BlackKing, this);
    }

    // Loop over all squares
    for(int square : (CurrentMove == WHITE) ? WhitePieces : BlackPieces){
        short piece = board[square];

        // Sliding piece
        if(GetMoveCapabilites(piece, SLIDE)){
            // Generate all possible sliding moves for that piece
            GenerateSlidingMoves(square, this);
        }
        else if(GetMoveCapabilites(piece, PAWN_MOVMENT)){
            GeneratePawnMovements(square, this);
        }
        else if(GetMoveCapabilites(piece, KNIGHT_MOVMENT)){
            GenerateKnightMovements(square, this);
        }
        else if(GetMoveCapabilites(piece, KING_MOVMENT)){
            GenerateKingMovements(square, this);
        }
    }

    // Store for if we want to reload it :)
    PreviousGeneration = CurrentMove;
    RegenNeeded = false;

    return MoveList;
}

bool Board::UpdateCheck(){
    // Generate next moves
    // std::list<Move> Moves = GenerateMoves();

    // // Reset check moves
    // Checkmoves = {};

    // // Reset check
    // Check = false;
    // CheckCount = 0;

    // // // See if any target king
    // for(Move m : Moves){
    //     if (m.Taking == KING){
    //         Check = true;
    //         CheckedColour = (CurrentMove == WHITE) ? BLACK : WHITE;

    //         // Add to checkmate paths
    //         // Checkmoves.push_back(m);

    //         CheckCount++;

    //         // return true; Dissabled as we want all routes of check
    //     }
    // }

    // Save current move list
    std::list<Move> Moves = std::list<Move>(MoveList);
    MoveList = {};

    // Reset check moves
    Checkmoves = {};

    // Reset check
    Check = false;
    CheckedColour = NULL_COLOUR;
    CheckCount = 0;

    // Get who we want in check
    int Square = (CurrentMove == WHITE) ? WhiteKing : BlackKing;

    // Generate All moves valid from the king and see if taking same move type
    
    // SLIDING HORIZONTAL
    SetMovements(board[Square], (SLIDE | SLIDE_HORIZONTAL)); // Configure correct
    GenerateSlidingMoves(Square, this);

    // SLIDING DIAGONAL
    SetMovements(board[Square], (SLIDE | SLIDE_DIAGONAL)); // Configure correct
    GenerateSlidingMoves(Square, this);

    // QUEEN Needs separate
    SetMovements(board[Square], (SLIDE)); // Configure correct
    GenerateSlidingMoves(Square, this);

    // PAWN MOVEMENTS
    SetMovements(board[Square], EN_PASSENT); // Configure correct
    GeneratePawnMovements(Square, this);

    // KNIGHT MOVEMENTS
    SetMovements(board[Square], KNIGHT_MOVMENT); // Configure correct
    GenerateKnightMovements(Square, this);

    SetMovements(board[Square], KING_MOVMENT); // Configure correct

    // Get who we want in check
    Square = (GetColour(board[Square]) == WHITE) ? BlackKing : WhiteKing;

    // Generate All moves valid from the king and see if taking same move type
    
    // SLIDING HORIZONTAL
    SetMovements(board[Square], SLIDE | SLIDE_HORIZONTAL); // Configure correct
    GenerateSlidingMoves(Square, this);

    // SLIDING DIAGONAL
    SetMovements(board[Square], SLIDE | SLIDE_DIAGONAL); // Configure correct
    GenerateSlidingMoves(Square, this);

    // QUEEN Needs separate
    SetMovements(board[Square], (SLIDE)); // Configure correct
    GenerateSlidingMoves(Square, this);

    // PAWN MOVEMENTS
    SetMovements(board[Square], EN_PASSENT); // Configure correct
    GeneratePawnMovements(Square, this);

    // KNIGHT MOVEMENTS
    SetMovements(board[Square], KNIGHT_MOVMENT); // Configure correct
    GenerateKnightMovements(Square, this);

    SetMovements(board[Square], KING_MOVMENT); // Configure correct

    // Now look over the movements
    for (Move m : MoveList){
        // Get the piece
        short p = board[m.End];

        if(IsNull(p)){ // Do nothing if null position
            continue;
        }

        // See if the piece there has said capabilities
        if((char)(GetMoveCapabilites(p, 0x7F)) == m.MoveType){

            // Check possible
            Check = true;
            CheckCount++;

            // Check colour
            CheckedColour = (GetColour(board[m.Start]) == WHITE) ? WHITE : BLACK;

            // Inverse move to follow formatting and save it
            Checkmoves.push_back(Move{m.End, m.Start, KING, MoveExtra{m.Extra.Type, m.Start}, m.MoveType});

            continue;
        }
    }

    // Restore move list
    MoveList = Moves;

    return Check; // Not in check
}

bool Board::UpdateMate(){
    // If no check then we dont need to try this
    if(!Check || CheckedColour != CurrentMove){
        return false;
    }

    // Generate next moves
    std::list<Move> Moves = GenerateMoves();
    std::list<Move> Checkables = Checkmoves;

    Mate = false;

    // TODO: ABSOFUCINGLOOTLY Breaks double check ;)

    for (Move m : Moves){
        for(Move c : Checkables){
            // Now see if we intersect the path
            if(c.MoveType & SLIDE_HORIZONTAL || c.MoveType == SLIDE && !(m.MoveType & KING_MOVMENT)){ // King cant do this method!
                int Increment = 0;

                // Find increment
                if(!(c.Start - c.End % 8)){ // Horizontal
                    Increment = 8;
                }
                else if (abs(c.Start - c.End) < 8){ // Verticle
                    Increment = 1;
                }
                else{
                    goto POST_IF_HORIZONAL_STATEMENT; // PAIN BUT YES
                }
                
                // Direction
                int Direction = Increment;
                if((c.Start - c.End) < 0){ // backwards
                    Direction *= -1;
                }

                // Now do we intersect
                if(!((c.End - m.End) % Increment)){
                    if((m.End - c.Start) * (m.End - c.End) <= 0){
                        return false;
                    }
                }

                // std::cout << "Horizontal" << std::endl;
                // std::cout << c.Start << " " << c.End << " " << m.Start << " " << m.End << " " << Direction << std::endl;
            } POST_IF_HORIZONAL_STATEMENT:
            if(c.MoveType & SLIDE_DIAGONAL || c.MoveType == SLIDE && !(m.MoveType & KING_MOVMENT)){ // No king escaping
                int Increment = 0;

                // Find increment
                if(!((c.End - c.Start) % 9)){ // Horizontal
                    Increment = 9;
                }
                else if(!((c.End - c.Start) % 7)){ // Verticle
                    Increment = 7;
                }
                else{
                    // Escape the if statment
                    goto POST_IF_DIAGONAL_STATEMENT; // I KNOW BUT FUCK YOUR OPINION, I am not making a web of nested if statements
                }
                
                // Direction
                int Direction = Increment;
                if((c.End - c.Start) < 0){ // backwards
                    Direction *= -1;
                }

                // Now do we intersect
                if(!((c.End - m.End) % Increment)){
                    if((m.End - c.Start) * (m.End - c.End) <= 0){
                        return false;
                    }
                }

                // std::cout << "Digaonal" << std::endl;
                // std::cout << c.Start << " " << c.End << " " << m.Start << " " << m.End << " " << Direction << std::endl;
            } POST_IF_DIAGONAL_STATEMENT:
            if(c.MoveType & KNIGHT_MOVMENT){
                if(m.End == c.Start){ // Need to kill night
                    // We can kill the knight
                    return false;
                }
                if(m.Start == c.End){ // Or king moves out of way
                    return false;
                }
            }
            if(c.MoveType & PAWN_MOVMENT){
                // Should work ?
                if(((c.End - c.Start) % 7) == 0 || ((c.End - c.Start) % 9) == 0){
                    
                }
                else{
                    
                }

                return false;
            }
        }
    }

    // for (Move m : Moves){
    //     bool Valid = MovePiece(m);

    //     if(Valid){
    //         UndoMove();
            // return false;
    //     }
    // }

    // Mate = true;

    // for (Move m : Moves){
        // bool Valid = MovePiece(m);
        // MovePiece(m, true);

        // if(Valid){ // All un-check moves are considered valid
        //     UndoMove();
        //     return false;
        // }

        // Now try regenerate all checked moves
    //     MoveList = {};
    //     for (Move m : Checkables){
    //         short piece = board[m.Start];

    //         if(GetColour(piece) == CheckedColour){
    //             continue; // We dont want to calculate our moves
    //         }

    //         // Sliding piece
    //         if(GetMoveCapabilites(piece, SLIDE)){
    //             // Generate all possible sliding moves for that piece
    //             GenerateSlidingMoves(m.Start, this);
    //         }
    //         else if(GetMoveCapabilites(piece, PAWN_MOVMENT)){
    //             GeneratePawnMovements(m.Start, this);
    //         }
    //         else if(GetMoveCapabilites(piece, KNIGHT_MOVMENT)){
    //             GenerateKnightMovements(m.Start, this);
    //         }
    //         else if(GetMoveCapabilites(piece, KING_MOVMENT)){
    //             GenerateKingMovements(m.Start, this);
    //         }
    //     }

    //     bool Escaped = true;

    //     // Now see if we still take king
    //     for(Move m : MoveList){
    //         if (m.Taking == KING){ // Not escaped check
    //             UndoMove();
    //             Escaped = false;
    //             break;
    //         }
    //     }

    //     if(Escaped){
    //         UndoMove();
    //         return false;
    //     }
    // }

    // Mate = true;

    return true;
}

int cast = 0;
int en = 0;

bool Board::MovePiece(Move m, bool Forced){ // REQUIRES A VALID MOVE TO BE PASSED IN
    // Can't take king
    if(m.Taking == KING){
        return false;
    }

    short startPiece = board[m.Start];
    Type startPieceType = GetType(startPiece);

    short endPiece = board[m.End];
    Type endPieceType = GetType(endPiece);

    // if(startPieceType == NULL_TYPE){ // TODO: Ensure not needed
    //     // BIG ISSUE
    //     std::cout << "ERR Cannot move null piece" << std::endl;
    // }

    // Piece List
    std::list<int>& startPieceList = (GetColour(startPiece) == WHITE) ? WhitePieces : BlackPieces;

    // Cache Move
    PlayedMoves.push_back(m);

    // Backupe piecese piee
    WhiteLists.push_back(std::list<int>(WhitePieces));
    BlackLists.push_back(std::list<int>(BlackPieces));

    // If taking piece
    if (endPieceType != NULL_TYPE){ // TODO: Assuming that colours are correct
        // Remove from corresponding list
        std::list<int>& endPieceList = (GetColour(endPiece) == WHITE) ? WhitePieces : BlackPieces;
        endPieceList.remove(m.End);
    }

    // Modify the Pieces List to update piece positioning
    startPieceList.remove(m.Start);
    startPieceList.push_back(m.End);

    // If the king is moving we need to adjust positions accordingly
    if(m.MoveType == KING_MOVMENT){
        if(m.Start == WhiteKing){
            WhiteKing = m.End;
        } else{
            BlackKing = m.End;
        }
    }

    // Backupe Borde
    short* BackBoard = new short[64];
    memcpy(BackBoard, board, 128);
    Boards.push_back(BackBoard);

    // Actual board swaps
    board[m.End] = board[m.Start];
    SET_NULL(board[m.Start]);

    // For en-passent and castling and piece general
    SET_MOVED(board[m.End]);

    // Require Move Regen
    RegenNeeded = true;

    // Move Extras
    if(m.Extra.Type){
        if(m.Extra.Type == SPECIAL_EN_PASSENT){
            board[m.Extra.Square] = board[m.Start]; // Kill piece
            std::list<int>& extraPieceList = (GetColour(board[m.Extra.Square]) == WHITE) ? WhitePieces : BlackPieces;
            extraPieceList.remove(m.Start); // Remove from pieces colours
        }
        else if(m.Extra.Type == SPECIAL_CASTLING_KING){
            // Move rook
            board[m.End-1] = board[m.Extra.Square];
            SET_NULL(board[m.Extra.Square]);
        }
        else if(m.Extra.Type == SPECIAL_CASTLING_QUEEN){
            // Move rook
            board[m.End+1] = board[m.Extra.Square];
            SET_NULL(board[m.Extra.Square]);
        }
        else if(m.Extra.Type == SPECIAL_PROMOTION){
            // TODO: Implement Picker
            SET_QUEEN(board[m.Extra.Square], GetColour(board[m.Extra.Square]));
        }
    }

    if(Forced){ // Skip all check management
        CurrentMove = (CurrentMove == WHITE) ? BLACK : WHITE;
        return true;
    }

    // Change next to move
    CurrentMove = (CurrentMove == WHITE) ? BLACK : WHITE;

    // Update check/mate/stalemate
    UpdateCheck();

    // TODO: Check is only considered when a move is attempted, until then the check is unknown!

    // See if check has changed
    if(Check && CheckedColour == GetColour(startPiece)){
        // Invalid as king in check
        UndoMove();

        // TODO: Checkmate see if any move could be made here to escape check!
        return false;
    }

    return true;
}

void Board::UndoMove(){
    // Simple check to see if moves have been played
    if (PlayedMoves.empty()){
        return; // No move to undo
    }

    // Remove previous cached move
    Move m = PlayedMoves.back();
    PlayedMoves.pop_back();

    // If the king is moving we need to adjust positions accordingly
    if(m.MoveType == KING_MOVMENT){
        if(m.End == WhiteKing){
            WhiteKing = m.Start;
        } else{
            BlackKing = m.Start;
        }
    }

    // Swap Board Pointers
    delete board; // Deletes mEMORY
    board = Boards.back();
    Boards.pop_back();

    // Re-add pieces
    WhitePieces = WhiteLists.back();
    BlackPieces = BlackLists.back();
    WhiteLists.pop_back();
    BlackLists.pop_back();

    // Swap Moves
    CurrentMove = (CurrentMove == WHITE) ? BLACK : WHITE;

    // Regen is no longer required
    RegenNeeded = false;

    return;
}
