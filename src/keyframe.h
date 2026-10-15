#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QVector3D>

struct Keyframe {
  int frame;
  QVector3D position, rotation, scale;
  Keyframe() = default;
  Keyframe(int frame, QVector3D position, QVector3D rotation, QVector3D scale) : frame(frame), position(position), rotation(rotation), scale(scale) {}
};

#endif // KEYFRAME_H
