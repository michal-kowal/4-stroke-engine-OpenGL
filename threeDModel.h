#include "common_header.h"

#ifndef THREEDMODEL
#define THREEDMODEL
class threeDModel {
public:
    std::vector<float> verts;
    std::vector<float> norms;
    std::vector<float> texCoords;
    std::vector<float> colors;
    int vertexCount;
    int movementDirection;
    threeDModel(const char * path, int direction);
    void render(ShaderProgram* sp, GLuint tex);

};
#endif