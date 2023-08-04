#pragma once

#include <chess/board.h>
#include <string>
#include <cstring>

struct ChessSetupPacket{
    const short PacketLength = 108;

    static const std::size_t SIZE = 1024; // Packet Size

    // Start header
    const char Header[17] = "Chess_Packet_0x3";

    // Other teams colour
    Colour col;

    // Game FEN
    char FEN[64] ; // NOTE SIZE LIMIT

    // Terminator
    const char Terminator[21] = "End_Chess_Packet_0x2";

    inline ChessSetupPacket()
    {
        // No Values
    }
    inline ChessSetupPacket(Colour Opponent, const char* FEN)
    {
        col = Opponent;
        strcpy(this->FEN, FEN);
    }

    // Loads the packet from a byte array
    inline void load_from_bytes(const std::array<char, SIZE>& data) {
        std::memcpy(this, data.data(), PacketLength);
    }

    // Converts the packet to a byte array
    inline std::array<char, SIZE> to_bytes() const {
        std::array<char, SIZE> bytes;
        std::memcpy(bytes.data(), this, PacketLength);
        return bytes;
    }
};

struct ChessPacket{
    const short PacketLength = 68;

    static const std::size_t SIZE = 1024; // Packet Size

    // Start header
    const char Header[17] = "Chess_Packet_0x1";

    Move PlayedMove;

    // Move Extra Conversions as we cant use pointers
    int MoveExtraFrom_Square; // TODO: NOT NEEDED
    int MoveExtraTo_Square;

    // Terminator
    const char Terminator[21] = "End_Chess_Packet_0x0";

    // Loads the packet from a byte array
    inline void load_from_bytes(const std::array<char, SIZE>& data) {
        std::memcpy(this, data.data(), PacketLength);
    }

    // Converts the packet to a byte array
    inline std::array<char, SIZE> to_bytes() const {
        std::array<char, SIZE> bytes;
        std::memcpy(bytes.data(), this, PacketLength);
        return bytes;
    }
};