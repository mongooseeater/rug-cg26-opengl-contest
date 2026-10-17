#ifndef ANIMATION_H
#define ANIMATION_H

#include "keyframe.h"
#include <QVector>
#include <QString>

class Animation {
public:
  Animation(const QString &filename);
  Keyframe getInterpolatedFrame(const int meshId, const int time);
  Animation() = default;
  QVector<QString> getMeshFilenames();
  QString getDiffuseTexturePath();
  double getFrameTime(); // = 1.0/frameRate
  int getLength();
private:
  std::vector<Keyframe> *sequences;
  QVector<QString> meshNames;
  QString diffuseTexturePath;
  int frameRate;
  int length;
  float interpolate(InterpolationMode interp, float delta);
};

#endif // ANIMATION_H
