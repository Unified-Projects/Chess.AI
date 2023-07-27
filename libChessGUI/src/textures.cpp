#include <libChessGUI/shaders.h>
#include <libChessGUI/textures.h>

using namespace GUI;

PieceTexture::PieceTexture(const char* filepath, unsigned int unitID) : UnitID(unitID){
    this->FilePath = filepath;

    // Read data
    this->Data = SOIL_load_image(filepath, &this->width, &this->height, NULL, SOIL_LOAD_RGBA);

    // Load the image to GLFW
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free Data
    SOIL_free_image_data(this->Data);
}

void PieceTexture::Bind(){
    glActiveTexture(GL_TEXTURE0 + this->UnitID);
    glBindTexture(GL_TEXTURE_2D, this->TextureID);

    Shaders::Toggle();
    Shaders::SendInt((int&)TextureID, "Texture");
    Shaders::Toggle();
}

void PieceTexture::UnBind(){
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

PieceTextureMapper::PieceTextureMapper(std::string ParentDirectory){ // TODO SYSTEM ARG
    // TODO: ISSUE :( not independent and inconsistent with other systems
    // Load White Textures
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wb.png").c_str(), 1));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wk.png").c_str(), 2));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wn.png").c_str(), 3));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wp.png").c_str(), 4));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wq.png").c_str(), 5));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/wr.png").c_str(), 6));

    // Load Black Textures
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/bb.png").c_str(), 7));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/bk.png").c_str(), 8));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/bn.png").c_str(), 9));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/bp.png").c_str(), 10));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/bq.png").c_str(), 11));
    Pieces.push_back(PieceTexture((ParentDirectory + "icons/br.png").c_str(), 12));
}

PieceTexture* PieceTextureMapper::GetTex(Type pT, Colour pC){
    int Offsets = (pC == WHITE) ? 0 : 6;

    if(pT == NULL_TYPE || pC == NULL_COLOUR){
        return nullptr;
    }

    switch (pT)
    {
    case PAWN:
        return &Pieces.at(Offsets+3);
    case ROOK:
        return &Pieces.at(Offsets+5);
    case KNIGHT:
        return &Pieces.at(Offsets+2);
    case BISHOP:
        return &Pieces.at(Offsets+0);
    case KING:
        return &Pieces.at(Offsets+1);
    case QUEEN:
        return &Pieces.at(Offsets+4);
    
    default:
        return nullptr;
    }
}