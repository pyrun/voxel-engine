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
#include "../xml/tinyxml2.h"

#define BLOCK_SIZE 16 // pxl image
//#define BLOCK_AMOUNT_SIDE 16
//#define RESERVE_BLOCKS 2048

enum block_side {
    BLOCK_SIDE_LEFT = 42,
    BLOCK_SIDE_RIGHT,
    BLOCK_SIDE_TOP,
    BLOCK_SIDE_BUTTOM,
    BLOCK_SIDE_FRONT,
    BLOCK_SIDE_BACK
};

class block_image  {
public:
    block_image();
    ~block_image();
    void loadImage( graphic* graphic);
    void setImageName( std::string name);
    int &getPosX() { return p_posx; }
    int &getPosY() { return p_posy; }
    SDL_Surface *getSurface() { return p_surface; }
    std::string getImageName() { return p_imagename; }
private:
    SDL_Surface* p_surface;
    int p_posx;
    int p_posy;
    std::string p_imagename;
};

class block {
public:
    block();
    ~block();

    void loadImage( graphic* t_graphic);
    void setID( int id) { p_id = id; }
    void setName( std::string name) { n_name = name; }
    void setAlpha( bool flag) { p_alpha = flag; }
    void setFile( std::string s_front, std::string s_back, std::string s_left, std::string s_right, std::string s_top, std::string s_bottom) {
        image_front.setImageName(s_front);
        image_back.setImageName(s_back);
        image_left.setImageName(s_left);
        image_right.setImageName(s_right);
        image_top.setImageName(s_top);
        image_bottom.setImageName(s_bottom);
    }
    block_image* getFront() { return &image_front; }
    block_image* getBack() { return &image_back; }
    block_image* getLeft() { return &image_left; }
    block_image* getRight() { return &image_right; }
    block_image* getUp() { return &image_top; }
    block_image* getDown() { return &image_bottom; }
    bool getLoadedImage() { return p_imageloaded; }
    int getID() { return p_id; }
    std::string getName() { return n_name; }
    bool getAlpha() { return p_alpha; }
protected:
private:
    // block
    int p_id;
    std::string n_name;
    bool p_alpha;
    block_image image_front;
    block_image image_back;
    block_image image_left;
    block_image image_right;
    block_image image_top;
    block_image image_bottom;
    bool p_imageloaded;
};

class block_list {
public:
    block_list( std::string Path);
    ~block_list();
    void draw( graphic* graphic);
    // Umrechnen wo welche textur liegt
    glm::vec2 getTexturByType( int Type, block_side side);
    block* get( int ID);
    block* getByName( std::string name);
    int getAmountblocks() { return (int)p_blocks.size(); }
private:
    std::string p_path;
    std::vector<block> p_blocks;
protected:
    bool GetSuffix(const std::string &file, const std::string &suffix);
    bool GetSuffix6502(const std::string& str, const std::string& end);
    bool FileExists(std::string StrFilename);
    void loadblock (std::string path, std::string name);
};
#endif // BLOCK_H
