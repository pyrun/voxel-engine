#ifndef CONFIG_H
#define CONFIG_H


class Config {
public:
    Config();
    virtual ~Config();
    void SetSupersampling( bool Set) { p_supersampling = Set; }
    void SetTransparency( bool Set) { p_transparency = Set; }

    bool GetSupersampling() { return p_supersampling; }
    bool GetTransparency() { return p_transparency; }

protected:
private:
    bool p_supersampling;
    bool p_transparency;
    bool p_fullscreen;
};

#endif // CONFIG_H
