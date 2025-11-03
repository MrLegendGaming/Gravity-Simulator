#ifndef SPHERE_H
#define _USE_MATH_DEFINES
#define SPHERE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

class Sphere {
public:
    static const int num_sectors = 30; // horizontal slices
    static const int num_stacks = 30;  // vertical slices
    std::vector<float> vertices;
    std::vector<GLuint> indices;
    GLuint vbo, ebo; // Vertex Buffer Object and Element Buffer Object

    Sphere(float radius) {
        generateVertices(radius);
        generateIndices();
        createBuffers();
    }

    ~Sphere() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void draw() {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

private:
    void generateVertices(float radius) {
        float sectorStep = 2 * M_PI / num_sectors;
        float stackStep = M_PI / num_stacks;

        for (int i = 0; i <= num_stacks; ++i) {
            float stackAngle = M_PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
            float xy = radius * cosf(stackAngle);       // r * cos(u)
            float z = radius * sinf(stackAngle);        // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= num_sectors; ++j) {
                float sectorAngle = j * sectorStep; // starting from 0 to 2pi

                // vertex position (x, y, z)
                float x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
                float y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
    }

    void generateIndices() {
        int k1, k2;
        for (int i = 0; i < num_stacks; ++i) {
            k1 = i * (num_sectors + 1);     // beginning of current stack
            k2 = k1 + num_sectors + 1;      // beginning of next stack

            for (int j = 0; j < num_sectors; ++j, ++k1, ++k2) {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (num_stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }

    void createBuffers() {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};


#endif