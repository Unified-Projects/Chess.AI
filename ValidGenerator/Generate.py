import chess
import json

def generate_fens_after_half_moves(half_moves):

    # CUSTOM FEN VERSION
    # board = chess.Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ")
    board = chess.Board()
    fens = []
    total_positions = 0

    def generate_fens_recursive(board, half_moves_left, position_count):
        nonlocal total_positions
        if half_moves_left == 0:
            fens.append(board.fen())
            position_count += 1
            total_positions += 1
            if total_positions % 10000 == 0:
                print(f"Generated {total_positions} FENs")
            return position_count

        for move in board.legal_moves:
            board.push(move)
            position_count = generate_fens_recursive(board, half_moves_left - 1, position_count)
            board.pop()

        return position_count

    total_positions = generate_fens_recursive(board, half_moves, total_positions)
    print(f"Generated {total_positions} FENs in total")
    return fens

# Example usage
half_moves = 4  # Number of half-moves
fens = generate_fens_after_half_moves(half_moves)

# Save the dictionary as a JSON file with nicely formatted structure
with open('Valid.txt', 'w', encoding='utf-8') as f:
    f.write(",".join(set(fens)))

print(f"Generated FENs saved in Valid.txt")