#ifndef ANIMATION_H
#define ANIMATION_H

#include "keyframe.h"
#include <QVector>
#include <QString>

#define DEFAULT_FRAMERATE 30
#define KEYFRAME_ENTRY_COUNT 11

class Animation {
public:
  Animation(const QString &filename);
  static Animation empty();

  Keyframe getInterpolatedFrame(const int meshId, const int time);
  Animation() = default;
  QVector<QString> getMeshFilenames();
  QString getDiffuseTexturePath();
  double getFrameTime(); // = 1.0/frameRate
  int getLength();
  void dispose();
private:
  std::vector<Keyframe> *sequences;
  QVector<QString> meshNames;
  QString diffuseTexturePath;
  int frameRate;
  int length;
  float interpolate(InterpolationMode interp, float delta);
};

#endif // ANIMATION_H
