//#define _USE_MATH_DEFINES
//#include <math.h>
//#include "Sphere.h"
//
//
//// Define the precision of the sphere.
//const int space = 20;
//
//// Define the radius of the sphere.
//const float radius = 1.0f;
//
//// Calculate the number of vertices.
//const int num_vertices = (180 / space + 1) * (360 / space + 1) * 3;
//
//// Create an array to hold the vertices.
//float vertices[num_vertices];
//
//void Sphere::CreateSphere(float vertices[])
//{
//    for (int theta = 0; theta <= 180; theta += space) {
//        for (int phi = 0; phi <= 360; phi += space) {
//            // Convert angles to radians.
//            float theta_rad = theta * M_PI / 180.0f;
//            float phi_rad = phi * M_PI / 180.0f;
//
//            // Calculate the vertex position.
//            float x = radius * sin(theta_rad) * cos(phi_rad);
//            float y = radius * sin(theta_rad) * sin(phi_rad);
//            float z = radius * cos(theta_rad);
//
//            // Add the vertex to the array.
//            vertices.push_back(x);
//            vertices.push_back(y);
//            vertices.push_back(z);
//        }
//
//    };
//}