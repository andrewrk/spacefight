#include "octahedron.h"

static const float invSqrt2 = 0.7071067811865475; // 1/sqrt(2)


void Octahedron::generate(int rowCount, std::vector<glm::vec3> &points, std::vector<GLuint> &indexes)
{

    float sideLen = 2 * invSqrt2 / (float)rowCount;


    glm::vec3 top(0, 1, 0);
    glm::vec3 bottomLeft(invSqrt2, 0, -invSqrt2);
    glm::vec3 downLeftDir = glm::normalize(bottomLeft - top);
    glm::vec3 zPos(0, 0, 1);
    glm::vec3 zNeg(0, 0, -1);
    glm::vec3 xNeg(-1, 0, 0);
    glm::vec3 xPos(1, 0, 0);

    for (int row = 0; row <= rowCount; row += 1) {
        glm::vec3 pt = top + downLeftDir * (row * sideLen);
        points.push_back(pt);
        for (int i = 1; i <= row; i += 1) {
            pt += zPos * sideLen;
            points.push_back(pt);
        }
        for (int i = 1; i <= row; i += 1) {
            pt += xNeg * sideLen;
            points.push_back(pt);
        }
        for (int i = 1; i <= row; i += 1) {
            pt += zNeg * sideLen;
            points.push_back(pt);
        }
        for (int i = 1; i < row; i += 1) {
            pt += xPos * sideLen;
            points.push_back(pt);
        }
    }

    // triangle on the cap
    indexes.push_back(1);
    indexes.push_back(0);
    indexes.push_back(2);

    indexes.push_back(0);
    indexes.push_back(3);

    indexes.push_back(0);
    indexes.push_back(4);

    indexes.push_back(0);
    indexes.push_back(1);

    int topIndex = 1;
    int bottomIndex = 5;
    for (int row = 1; row < rowCount; row += 1) {
        int triangleCount = row + 1;
        int rowStartTop = topIndex;
        int rowStartBottom = bottomIndex;

        indexes.push_back(bottomIndex);

        indexes.push_back(bottomIndex++);
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount - 1; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        indexes.push_back(rowStartTop);
        indexes.push_back(rowStartBottom);
    }
    // add all the points except the bottom row to points again, flipping the Y
    int pointsMidway = points.size();
    int pointsExceptLastRow = pointsMidway - rowCount * 4;
    for (int i = pointsExceptLastRow - 1; i >= 0; i -= 1) {
        glm::vec3 pt = points[i];
        pt[1] = -pt[1];
        points.push_back(pt);
    }
    // add all the indexes again to indexes
    int indexEnd = indexes.size();
    for (int i = 0; i < indexEnd; i += 1) {
        int index = points.size() - 1 - indexes[i];
        // if the index is trying to be a bottom row index, adjust it accordingly
        if (index < pointsMidway) {
            index = pointsExceptLastRow + (pointsMidway - index - 1);
        }
        indexes.push_back(index);
    }

}
