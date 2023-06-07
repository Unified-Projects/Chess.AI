#include <chess/board.h>

// TEMPORARY INCLUDES
    #include <iostream>
        #define UNIMPLEMENTED std::cout << "Unimplemented " << __LINE__ << std::endl;
//

// Static definition for mapping
std::map<char, Piece> Board::pieceMapper = {
        {'r', Rook(BLACK)}, {'R', Rook(WHITE)},
        {'n', Knight(BLACK)}, {'N', Knight(WHITE)},
        {'p', Pawn(BLACK)}, {'P', Pawn(WHITE)},
        {'b', Bishop(BLACK)}, {'B', Bishop(WHITE)},
        {'q', Queen(BLACK)}, {'Q', Queen(WHITE)},
        {'k', King(BLACK)}, {'K', King(WHITE)},
    };
std::map<char, Type> Board::typeMapper = {
        {'r', ROOK}, {'R', ROOK},
        {'n', KNITGHT}, {'N', KNITGHT},
        {'p', PAWN}, {'P', PAWN},
        {'b', BISHOP}, {'B', BISHOP},
        {'q', QUEEN}, {'Q', QUEEN},
        {'k', KING}, {'K', KING},
    };

void Board::SetPiece(int X, int Y, Piece p){
    // Load to the board
        // YX as in row then collumn
        // Yes we map 8 to 1 and 1 to 7 just as it's easier
    board[Y][X] = p;
    board[Y][X].X = X;
    board[Y][X].Y = Y;
    board[Y][X].b = this; // So that pieces can acces the current board (Instead of global so that multiple boards can be active at once (Training AI))
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
            LocalX += *FEN_STR - 48;
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
        SetPiece(LocalX, LocalY, std::move(pieceMapper[*FEN_STR]));

        // Move horizonatally
        LocalX++;
    }
}

void Board::LogBoard(){
    // Iterate both over rows and columns
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            // Get the current pieces colour and type
            Type t = board[i][j].t;
            Colour c = board[i][j].c;

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
}