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
        {NULL_TYPE, ' '}
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
    // if (CurrentMove == PreviousGeneration){
    //     return MoveList;
    // }

    MoveList = {};

    // TODO: Promotion issue where queen cant move after promoting

    // Loop over all squares
    // for (int Square = 0; Square < 64; Square++){ - Optimised Out
    for(int square : (CurrentMove == WHITE) ? WhitePieces : BlackPieces){
        // Piece* p = board[Square]; // Select Piece - Optimised Out

        // Ensure its the pieces turn to move - Optimised Out
        // if(p->c != CurrentMove){
        //     continue;
        // }

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
    // PreviousGeneration = CurrentMove;

    return MoveList;
}

bool Board::UpdateCheck(){
    // Generate next moves
    std::list<Move> Moves = GenerateMoves();

    // Reset check moves
    Checkmoves = {};

    // Reset check
    Check = false;

    // See if any target king
    for(Move m : Moves){
        if (m.Taking == KING){
            Check = true;
            CheckedColour = (CurrentMove == WHITE) ? BLACK : WHITE;

            // Add to checkmate paths
            Checkmoves.push_back(m);

            // return true; Dissabled as we want all routes of check
        }
    }

    return Check; // Not in check
}

bool Board::UpdateMate(){
    // Generate next moves
    std::list<Move> Moves = GenerateMoves();

    for (Move m : Moves){
        bool Valid = MovePiece(m);

        if(Valid){ // All un-check moves are considered valid
            UndoMove();
            return false;
        }
    }

    return true;
}

int cast = 0;
int en = 0;

bool Board::MovePiece(Move m){ // REQUIRES A VALID MOVE TO BE PASSED IN
    // Can't take king
    if(m.Taking == KING){
        return false;
    }

    if(GetType(board[m.Start]) == NULL_TYPE){
        // BIG ISSUE
        std::cout << "ERR Cannot move null piece" << std::endl;
    }

    PlayedMoves.push_back(m);

    // If taking piece
    if (GetType(board[m.End]) != NULL_TYPE){ // TODO: Assuming that colours are correct
        // Remove from corresponding list
        ((GetColour(board[m.End]) == WHITE) ? WhitePieces : BlackPieces).remove(m.End);
    }

    // Backupe Borde
    short* BackBoard = new short[64];
    // std::memset(BackBoard, 0, 128); // Not needed?
    memcpy(BackBoard, board, 128);
    Boards.push_back(BackBoard);

    // Backupe piecese piee
    WhiteLists.push_back(std::list<int>(WhitePieces));
    BlackLists.push_back(std::list<int>(BlackPieces));

    // Actual board swaps
    board[m.End] = board[m.Start];
    SET_NULL(board[m.Start]);

    // For en-passent and castling and piece general
    SetHasMoved(board[m.End], 1);

    // Move Extras
    if(m.Extra.Type){
        if(m.Extra.Type == SPECIAL_EN_PASSENT){
            board[m.Extra.Square] = board[m.Start]; // Kill piece
            ((GetColour(board[m.Extra.Square]) == WHITE) ? WhitePieces : BlackPieces).remove(m.Start); // Remove from pieces colours
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

    // Change next to move
    CurrentMove = (CurrentMove == WHITE) ? BLACK : WHITE;

    // Update check/mate/stalemate
    UpdateCheck();

    // TODO: Check is only considered when a move is attempted, until then the check is unknown!

    // See if check has changed
    if(Check && CheckedColour == ((CurrentMove == WHITE) ? BLACK : WHITE)){
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

    // Swap Board Pointers
    delete board; // Deletes mEMORY
    board = Boards.back();
    Boards.pop_back();

    // Re-add pieces
    WhitePieces = WhiteLists.back();
    BlackPieces = BlackLists.back();
    WhiteLists.pop_back();
    BlackLists.pop_back();

    return;
}
