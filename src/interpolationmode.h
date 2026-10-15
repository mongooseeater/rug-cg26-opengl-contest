#ifndef INTERPOLATIONMODE_H
#define INTERPOLATIONMODE_H

enum InterpolationMode {
  Constant = 0,
  Linear = 1,
  Sine = 2,
  Quad = 3,

  None = -1 // used only when not applicable, i.e. interpolated frames
};

#endif // INTERPOLATIONMODE_H
