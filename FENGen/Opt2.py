import chess
import json

def generate_all_fens_json():
    initial_position = chess.Board()
    fens = []

    stack = [(initial_position.copy(), [])]

    while stack:
        position, moves = stack.pop()

        legal_moves = position.legal_moves

        if not legal_moves:
            fens.append(position.fen())
        else:
            for move in legal_moves:
                new_position = position.copy()
                new_position.push(move)
                stack.append((new_position, moves + [move]))

    fens_data = {"fens": fens}

    with open("all_fens.json", "w", encoding="utf-8") as f:
        json.dump(fens_data, f, indent=4)

generate_all_fens_json()