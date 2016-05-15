#ifndef CONFIG_H
#define CONFIG_H


class Config {
public:
    Config();
    virtual ~Config();
    void SetSupersampling( bool Set) { m_supersampling = Set; }
    void SetTransparency( bool Set) { m_transparency = Set; }

    bool GetSupersampling() { return m_supersampling; }
    bool GetTransparency() { return m_transparency; }

protected:
private:
    bool m_supersampling;
    bool m_transparency;
    bool m_fullscreen;
};

#endif // CONFIG_H
