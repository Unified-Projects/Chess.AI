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

Board::Board() {
    // Check management
    Check = false;
    Stale = false;
    Mate = false;

    // Color
    CheckedColour = NULL_COLOUR;
    CurrentMove = WHITE;

    // King savings
    WhiteKing = nullptr;
    BlackKing = nullptr;

    // Pieces
    WhitePieces = {};
    BlackPieces = {};

    MoveList = {};
    PreviousGeneration = NULL_COLOUR; // Caching

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
    int LocalRank = 7;
    int LocalFile = 0;

    // TODO: Fix loading of board, currently backwards
    // // Iterate over the FEN inputted notation for parsing
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
                board[(LocalRank * 8) + LocalFile + i] = new Piece();
                board[(LocalRank * 8) + LocalFile + i]->Square = (LocalRank * 8) + LocalFile + i;
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

        // Section end so we end
        if (*FEN_STR == ' ') {
            break;
        }

        // Piece assignment according to map
        Piece* newPiece = pieceMapper[*FEN_STR]->Clone();
        board[(LocalRank * 8) + LocalFile] = newPiece;
        newPiece->Square = (LocalRank * 8) + LocalFile;

        if (newPiece->t == KING){
            if (newPiece->c == WHITE){
                WhiteKing = newPiece;
            }
            else{
                BlackKing = newPiece;
            }
        }

        if (newPiece->c == WHITE){
            WhitePieces.push_back(newPiece);
        }
        else{
            BlackPieces.push_back(newPiece);
        }

        // Move horizonatally
        LocalFile++;
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
            Type t = board[(i*8) + j]->t;
            Colour c = board[(i*8) + j]->c;

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
            Type type = board[(y*8) + x]->GetT();
            Colour colour = board[(y*8) + x]->GetC();

            if (type == NULL_TYPE) {
                nullCounter++;
                continue;
            }
            if (nullCounter) {
                fen += std::to_string(nullCounter);
                nullCounter = 0;
            }
            char t = Board::typeMapper[type] + ((colour == WHITE) ? ('A' - 'a') : 0);
            fen += t;

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

    // Loop over all squares
    // for (int Square = 0; Square < 64; Square++){ - Optimised Out
    for(Piece* p : (CurrentMove == WHITE) ? WhitePieces : BlackPieces){
        // Piece* p = board[Square]; // Select Piece - Optimised Out

        // Ensure its the pieces turn to move - Optimised Out
        // if(p->c != CurrentMove){
        //     continue;
        // }

        // Sliding piece
        if(p->MovingCapabilites & SLIDE){
            // Generate all possible sliding moves for that piece
            GenerateSlidingMoves(p->Square, p, this);
        }
        else if(p->MovingCapabilites & PAWN_MOVMENT){
            GeneratePawnMovements(p->Square, p, this);
        }
        else if(p->MovingCapabilites & KNIGHT_MOVMENT){
            GenerateKnightMovements(p->Square, p, this);
        }
        else if(p->MovingCapabilites & KING_MOVMENT){
            GenerateKingMovements(p->Square, p, this);
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

    return false; // Not in check
}

bool Board::UpdateMate(){
    // Generate next moves
    std::list<Move> Moves = GenerateMoves();

    for (Move m : Moves){
        bool Valid = MovePiece(m);

        if(Valid){ // All non-check moves are considered valid
            UndoMove();
            return false;
        }
    }

    return true;
}

bool Board::MovePiece(Move m){ // REQUIRES A VALID MOVE TO BE PASSED IN
    // Can't take king
    if(m.Taking == KING){
        return false;
    }

    PlayedMoves.push_back(MoveCache{board[m.Start], board[m.End], m, m.Extra});

    // If taking piece
    if (board[m.End]->t != NULL_TYPE){
        // Remove from corresponding list
        ((board[m.End]->c == WHITE) ? WhitePieces : BlackPieces).remove(board[m.End]);
    }

    // TODO: PROMOTION DOES NOT WORK!

    // Actual board swaps
    board[m.End] = board[m.Start];
    board[m.Start] = new Piece();

    // For en-passent and castling and piece general
    board[m.End]->moveCount++;
    board[m.End]->Square = m.End;

    // Move Extras
    if(m.Extra.type){
        if(m.Extra.type == SPECIAL_EN_PASSENT){
            board[m.Extra.square] = m.Extra.To; // Kill piece
            ((board[m.Extra.square]->c == WHITE) ? WhitePieces : BlackPieces).remove(m.Extra.From); // Remove from pieces colours
        }
        else if(m.Extra.type = SPECIAL_CASTLING){
            // Change Cache to follow new move extra :)
            PlayedMoves.back().Extra.square = m.Extra.From->Square;

            // Move rook
            board[m.Extra.From->Square] = m.Extra.To;
            board[m.Extra.square] = m.Extra.From;
            m.Extra.From->Square = m.Extra.square;
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

    // Lets to a simple usage
    MoveCache Move = PlayedMoves.back();
    PlayedMoves.pop_back();
    board[Move.move.Start] = Move.MovedPiece;
    board[Move.move.End] = Move.TargetPiece;

    // Move Extras
    if(Move.Extra.type){
        if(Move.Extra.type == SPECIAL_EN_PASSENT){
            board[Move.Extra.square] = Move.Extra.From;
            ((board[Move.Extra.square]->c == WHITE) ? WhitePieces : BlackPieces).push_back(Move.Extra.From);
        }
        else if(Move.Extra.type == SPECIAL_CASTLING){
            // Move rook
            board[Move.Extra.From->Square] = new Piece();
            board[Move.Extra.square] = Move.Extra.From;
            Move.Extra.From->Square = Move.Extra.square;
        }
    }

    // Resort Check
    UpdateCheck();

    // If piece taken
    if (board[Move.move.End]->t != NULL_TYPE){
        // Restore to corresponding list
        ((board[Move.move.End]->c == WHITE) ? WhitePieces : BlackPieces).push_back(board[Move.move.End]);
    }

    // Decrease move count and restore positions
    Move.MovedPiece->moveCount--;
    Move.MovedPiece->Square = Move.move.Start;

    CurrentMove = (CurrentMove == WHITE) ? BLACK : WHITE;

    // Make require regen move
    PreviousGeneration = NULL_COLOUR;

    return;
}
