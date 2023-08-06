import chess
import json
from collections import Counter

def generate_fens_after_half_moves(half_moves):

    board = chess.Board()
    checkmate_positions = []
    total_positions = 0

    def generate_fens_recursive(board, half_moves_left, position_count):
        nonlocal total_positions
        if half_moves_left == 0:
            if board.is_checkmate():
                checkmate_positions.append(board.fen())
            position_count += 1
            total_positions += 1
            if total_positions % 10000 == 0:
                print(f"Generated {total_positions} positions")
            return position_count

        for move in board.legal_moves:
            board.push(move)
            position_count = generate_fens_recursive(board, half_moves_left - 1, position_count)
            board.pop()

        return position_count

    total_positions = generate_fens_recursive(board, half_moves, total_positions)
    print(f"Generated {total_positions} positions in total")
    return checkmate_positions

# Example usage
half_moves = 4  # Number of half-moves
fens = generate_fens_after_half_moves(half_moves)

# Piece type to name mapping
piece_names = ["", "Pawn", "Knight", "Bishop", "Rook", "Queen", "King"]

# For each checkmate FEN, print the FEN, the board, and the checkmating piece
checkmate_counts = Counter()
for fen in fens:
    board = chess.Board(fen)
    print(f"\nCheckmate position: {fen}")
    print(board)
    # Get the checkmating piece
    checkers = board.checkers()
    for checker in checkers:  # There might be more than one
        piece = board.piece_at(checker)
        checkmate_counts[piece_names[piece.piece_type]] += 1
        print(f"Checkmating piece: {piece_names[piece.piece_type]} at {chess.square_name(checker)}")

# Print checkmate statistics
print("\nCheckmate statistics:")
for piece, count in checkmate_counts.most_common():
    print(f"{piece}: {count}")

# Save the dictionary as a JSON file with nicely formatted structure
# with open('CheckmatePositions.txt', 'w', encoding='utf-8') as f:
#     f.write(",".join(set(fens)))

# print(f"\nCheckmate positions saved in CheckmatePositions.txt")
