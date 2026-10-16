#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector3D>

#include "model.h"
#include "animation.h"

#define MAX_MESH_COUNT 8
// "real" frametime for the underlying animation system in milliseconds.
// this is the interval at which paintGL() ergo animateFrame() will be run;
// this has to be small enough to allow for flexible framerates for animations.
#define REAL_FRAME_TIME_MS 10

/**
 * @brief The MainView class is resonsible for the actual content of the main
 * window.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

 public:
  MainView(QWidget *parent = nullptr);
  ~MainView() override;

  // Functions for widget input events
  void setRotation(int rotateX, int rotateY, int rotateZ);
  void setScale(float scale);
  void setCurrentFrameNum(int newFrameNum);

  void loadAnimation(const QString filename);
  void togglePlayPause();

 protected:
  void initializeGL() override;
  void resizeGL(int newWidth, int newHeight) override;
  void paintGL() override;

  // Functions for keyboard input events
  void keyPressEvent(QKeyEvent *ev) override;
  void keyReleaseEvent(QKeyEvent *ev) override;

  // Function for mouse input events
  void mouseDoubleClickEvent(QMouseEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *ev) override;
  void mousePressEvent(QMouseEvent *ev) override;
  void mouseReleaseEvent(QMouseEvent *ev) override;
  void wheelEvent(QWheelEvent *ev) override;

 private slots:
  void onMessageLogged(QOpenGLDebugMessage Message);

 private:
  void createShaderProgram();
  void loadMeshes(const QVector<QString> &filenames);
  void destroyModelBuffers();
  void updateProjectionTransform();
  void updateModelTransforms();
  QVector<quint8> imageToBytes(const QImage &image);

  void animateFrame();

  QOpenGLDebugLogger debugLogger;
  QTimer timer;  // timer used for animation

  QOpenGLShaderProgram shader;

  // texture
  GLuint meshTexture;

  // Mesh values
  GLuint meshVAO;
  GLuint meshPositionVBO, meshNormalVBO, meshTexCoordVBO, meshIdVBO;
  GLuint meshSize;
  QMatrix4x4 meshTransforms[MAX_MESH_COUNT];
  int meshCount;

  // Transforms
  float scales[MAX_MESH_COUNT];
  QVector3D rotations[MAX_MESH_COUNT];
  QMatrix4x4 projectionTransform;
  QMatrix3x3 normalTransforms[MAX_MESH_COUNT];

  // animation
  Animation animation;
  int currentFrame = 0;
  QElapsedTimer animationTimer;
  bool animationPaused = true;

  // global transform ("over entire scene")
  float globalScale = 1.0F;
  QVector3D globalRotation;
};

#endif  // MAINVIEW_H
