#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QVector3D>

#include "interpolationmode.h"

struct Keyframe {
  int frame;
  InterpolationMode interp;
  QVector3D position, rotation, scale;
  Keyframe() = default;
  Keyframe(int frame, InterpolationMode interp, QVector3D position, QVector3D rotation, QVector3D scale)
    : frame(frame), interp(interp), position(position), rotation(rotation), scale(scale) {}
};

#endif // KEYFRAME_H
