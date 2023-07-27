#pragma once
#include <libChessGUI/configure.h>
#include <string>
#include <vector>
#include <stdint.h>

#include <chess/piece.h>

namespace GUI
{
    class PieceTexture{ // TODO: Deleting?
    public:
        std::string FilePath;
        uint8_t* Data;

        int width;
        int height;
        unsigned int TextureID;
        unsigned int UnitID;

        PieceTexture(const char* filepath, unsigned int unitID);

        void Bind();

        void UnBind();
    };

    class PieceTextureMapper{ // TODO: Deleting?
    public:
        std::vector<PieceTexture> Pieces;

        PieceTextureMapper(std::string ParentDirectory = "../../"); // TODO HMMMMMMMMM

        PieceTexture* GetTex(Type pT, Colour pC);
    };
} // namespace GUI::Textures
