/*
 * Author: Emoy Kim
 * E-mail: emoy.kim_AT_gmail.com
 * 
 * This code is a free software; it can be freely used, changed and redistributed.
 * If you use any version of the code, please reference the code.
 * 
 */

#pragma once

#include "OpenCVLinker.h"
#include "OpenGLLinker.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

using namespace std;
using namespace cv;
using namespace glm;

inline void printMatrix(const mat4 &matrix, const string &name)
{
   cout << "-------- " << name << " -------------------------\n";
   cout << matrix[0][0] << " " << matrix[1][0] << " " << matrix[2][0] << " " << matrix[3][0] << "\n";
   cout << matrix[0][1] << " " << matrix[1][1] << " " << matrix[2][1] << " " << matrix[3][1] << "\n";
   cout << matrix[0][2] << " " << matrix[1][2] << " " << matrix[2][2] << " " << matrix[3][2] << "\n";
   cout << matrix[0][3] << " " << matrix[1][3] << " " << matrix[2][3] << " " << matrix[3][3] << "\n";
   cout << "-------------------------------------------------\n\n";
}