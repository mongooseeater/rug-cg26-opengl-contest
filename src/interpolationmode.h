#ifndef INTERPOLATIONMODE_H
#define INTERPOLATIONMODE_H

enum InterpolationMode {
  Constant = 0,
  Linear = 1,
  
  Sine = 2,
  SineIn = 3,
  SineOut = 4,

  Quad = 5,
  QuadIn = 6,
  QuadOut = 7,

  Bounce = 8,

  None = -1 // used only when not applicable, i.e. interpolated frames
};

#endif // INTERPOLATIONMODE_H
