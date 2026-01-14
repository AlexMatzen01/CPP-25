#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int unit = 0) const;
    void unbind() const;

    unsigned int getID() const { return m_id; }

private:
    unsigned int m_id;
    int m_width, m_height, m_nrChannels;
};

#endif // TEXTURE_H
