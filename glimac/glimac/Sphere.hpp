#pragma once

#include <vector>

#include "common.hpp"

namespace glimac {

// Represents a discretized sphere centered at (0, 0, 0) (in its local coordinate system)
// Its vertical axis is (0, 1, 0) and its transverse axes are (1, 0, 0) and (0, 0, 1)
class Sphere {
    // Allocates and builds the data (implementation in the .cpp)
    void build(GLfloat radius, GLsizei discLat, GLsizei discLong);

public:
    // Constructor: allocate data array and build vertex attributes
    Sphere(GLfloat radius, GLsizei discLat, GLsizei discLong):
        m_nVertexCount(0) {
        build(radius, discLat, discLong); // build (see .cpp)
    }



    // Returns the pointer to the data
    const ShapeVertex* getDataPointer() const {
        return &m_Vertices[0];
    }
    
    // Returns the number of vertices
    GLsizei getVertexCount() const {
        return m_nVertexCount;
    }

private:
    std::vector<ShapeVertex> m_Vertices;
    GLsizei m_nVertexCount; // number of vertices
};
    
}