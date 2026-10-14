#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

/**
 * @brief Represents a single vertex with coordinates and an rgb color.
 */
struct Vertex {
  QVector3D coords;
  QVector3D color;
  QVector2D textureCoords;

  Vertex() = default;
  Vertex(QVector3D coords, QVector3D color, QVector2D textureCoords) : coords(coords), color(color), textureCoords(textureCoords) {}
};

#endif  // VERTEX_H
