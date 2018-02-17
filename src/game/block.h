#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <math.h>
#include "../graphic/graphic.h"
#include "../system/tinyxml2.h"

#define BLOCK_SIZE 16
#define BLOCK_AMOUNT_SIDE 6
#define RESERVE_BLOCKS 2048

class BlockImage  {
public:
    BlockImage();
    ~BlockImage();
    void LoadImage( graphic* graphic);
    void SetImageName( std::string name);
    int &getPosX() { return p_posx; }
    int &getPosY() { return p_posy; }
    SDL_Surface *getSurface() { return p_surface; }
    std::string GetImageName() { return p_imagename; }
private:
    SDL_Surface* p_surface;
    int p_posx;
    int p_posy;
    std::string p_imagename;
};

class Block {
public:
    Block();
    ~Block();

    void LoadImage( graphic* t_graphic);
    void SetID( int id) { i_id = id; }
    void SetName( std::string name) { n_name = name; }
    void SetAlpha( bool flag) { p_alpha = flag; }
    void SetFile( std::string s_front, std::string s_back, std::string s_left, std::string s_right, std::string s_top, std::string s_bottom) {
        image_front.SetImageName(s_front);
        image_back.SetImageName(s_back);
        image_left.SetImageName(s_left);
        image_right.SetImageName(s_right);
        image_top.SetImageName(s_top);
        image_bottom.SetImageName(s_bottom);
    }
    BlockImage* getFront() { return &image_front; }
    BlockImage* getBack() { return &image_back; }
    BlockImage* getLeft() { return &image_left; }
    BlockImage* getRight() { return &image_right; }
    BlockImage* getUp() { return &image_top; }
    BlockImage* getDown() { return &image_bottom; }
    bool getLoadedImage() { return p_imageloaded; }
    int getID() { return i_id; }
    std::string getName() { return n_name; }
    bool getAlpha() { return p_alpha; }
protected:
private:
    // Block
    int i_id;
    std::string n_name;
    bool p_alpha;
    BlockImage image_front;
    BlockImage image_back;
    BlockImage image_left;
    BlockImage image_right;
    BlockImage image_top;
    BlockImage image_bottom;
    bool p_imageloaded;
};

class BlockList {
public:
    BlockList( std::string Path);
    ~BlockList();
    void Draw( graphic* graphic);
    // Umrechnen wo welche textur liegt
    glm::vec2 GetTexturByType( int Type, int Side);
    Block* GetBlock( int ID);
    Block* GetBlockID( std::string name);
    int GetAmountBlocks() { return (int)List.size(); }
private:
    std::string Path;
    std::vector<Block> List;
protected:
    bool GetSuffix(const std::string &file, const std::string &suffix);
    bool GetSuffix6502(const std::string& str, const std::string& end);
    bool FileExists(std::string StrFilename);
    void LoadBlock (std::string Path, std::string Name);
};
#endif // BLOCK_H
