#include "mainview.h"

#include <QDateTime>
#include <QQuaternion>

/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "MainView constructor";

  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));

  for (int i = 0; i < MAX_MESH_COUNT; i++){
    rotations[i] = QVector3D(0.0, 0.0, 0.0);
    scales[i] = 1.0F;
  }
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 */
MainView::~MainView() {
  qDebug() << "MainView destructor";

  makeCurrent();

  destroyModelBuffers();
  glDeleteTextures(1, &meshTexture);
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL() {
  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  }

  QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glEnable(GL_CULL_FACE);

  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  // Set the color to be used by glClear. This is, effectively, the background
  // color.
  glClearColor(0.37f, 0.42f, 0.45f, 0.0f);

  createShaderProgram();
  loadMeshes(QVector<QString>({":/models/cat.obj", ":/models/cat.obj"}));

  // Initialize transformations
  updateProjectionTransform();
  updateModelTransforms();
}

/**
 * @brief MainView::createShaderProgram Creates a new shader program with a
 * vertex and fragment shader.
 */
void MainView::createShaderProgram() {
  // add vert+frag shaders
  shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vert.glsl");
  shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/frag.glsl");
  // link shader program
  shader.link();
}

/**
 * @brief MainView::loadMeshes Loads meshes from a list of filenames.
 * @param filename Filenames of the meshes to be loaded. They will be assigned sequential mesh IDs starting from 0.
 */
void MainView::loadMeshes(const QVector<QString> &filenames) {
  qDebug() << ":: Scene load";
  QVector<Model> models;
  meshCount = filenames.size();
  qDebug() << ":: (Sub)mesh count:" << meshCount;
  if ((meshCount < 0) || (meshCount > MAX_MESH_COUNT)) {
    qDebug() << "ERROR: Unsupported/invalid mesh count!";
    throw 1;
  }
  models.reserve(meshCount);
  for (int i = 0; i < meshCount; i++) models.append(Model(filenames[i]));

  // find total size of "supermesh" obtained by combining model meshes
  meshSize = 0;
  for (auto m : models) meshSize += m.getCoords().size();
  qDebug() << ":: Total supermesh size:" << meshSize;

  // reserve buffers for soon-to-be VBOs
  QVector<QVector3D> meshCoords;
  QVector<QVector3D> meshNormals;
  QVector<QVector2D> meshTexCoords;
  QVector<int> meshIds;
  meshCoords.reserve(meshSize);
  meshNormals.reserve(meshSize);
  meshTexCoords.reserve(meshSize);
  meshIds.reserve(meshSize);

  // combine individual mesh data into one "supermesh"
  for (int i = 0; i < meshCount; i++) {
    meshCoords.append(models[i].getCoords());
    meshNormals.append(models[i].getNormals());
    meshTexCoords.append(models[i].getTextureCoords());
    // mesh ids of vertices = id of this mesh
    auto size = models[i].getCoords().size();
    for (int x = 0; x < size; x++) meshIds.append(i);
  }

  // Generate VAO
  glGenVertexArrays(1, &meshVAO);
  glBindVertexArray(meshVAO);

  // Generate VBOs (one per vertex attribute)
  glGenBuffers(1, &meshPositionVBO);
  glGenBuffers(1, &meshNormalVBO);
  glGenBuffers(1, &meshTexCoordVBO);
  glGenBuffers(1, &meshIdVBO);

  // vertex coordinates -> copy into position VBO
  glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
  glBufferData(GL_ARRAY_BUFFER,
               meshCoords.size() * sizeof(QVector3D),
               meshCoords.data(),
               GL_STATIC_DRAW);

  // vertex coordinates to location 0
  // Note: glVertexAttribPointer implicitly reference the VBO currently bound to GL_ARRAY_BUFFER
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D),
                        reinterpret_cast<GLvoid *>(0));
  glEnableVertexAttribArray(0);

  // normals -> copy into normal VBO
  glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
  glBufferData(GL_ARRAY_BUFFER,
               meshNormals.size() * sizeof(QVector3D),
               meshNormals.data(),
               GL_STATIC_DRAW);

  // normal values to location 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D),
                        reinterpret_cast<GLvoid *>(0));
  glEnableVertexAttribArray(1);

  // UV data (texture coordinates) -> texture coordinates VBO
  glBindBuffer(GL_ARRAY_BUFFER, meshTexCoordVBO);
  glBufferData(GL_ARRAY_BUFFER,
                meshTexCoords.size() * sizeof(QVector2D),
                meshTexCoords.data(),
                GL_STATIC_DRAW);

  // UV/tex coordinates to location 2
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D),
                         reinterpret_cast<GLvoid *>(0));
  glEnableVertexAttribArray(2);

  // mesh ids -> copy into meshID VBO
  glBindBuffer(GL_ARRAY_BUFFER, meshIdVBO);
  glBufferData(GL_ARRAY_BUFFER,
               meshIds.size() * sizeof(int),
               meshIds.data(),
               GL_STATIC_DRAW);

  // mesh ids to location 3
  // IPointer -> "is integer, treat as integer"
  glVertexAttribIPointer(3, 1, GL_INT, sizeof(int),
                        reinterpret_cast<GLvoid *>(0));
  glEnableVertexAttribArray(3);

  // Generally good practice to unbind the buffers to prevent anything after
  // this from accidentally modifying it.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Generate texture
  glGenTextures(1, &meshTexture);

  // Set up texture parameters
  glBindTexture(GL_TEXTURE_2D, meshTexture);
  // horizontal-vertical wrapping: "clamp to edge"
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // minifying-magnifying: linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // upload (diffuse) texture data
  auto qImageTexture = QImage(":/textures/cat_diff.png");
  auto qVectorTexture = imageToBytes(qImageTexture);

  glTexImage2D(GL_TEXTURE_2D, 0,  // 2D texture, no mipmapping
                GL_RGBA8,         // target texture format: RGBA8 (8 bits per channel)
                qImageTexture.width(), qImageTexture.height(),
                0,                // texture border=0
                GL_RGBA,          // pixels being sent are in RGBA format
                GL_UNSIGNED_BYTE, qVectorTexture.data()); // = texture data unsigned bytes, from this array
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL() {
  // Clear the screen before rendering
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // select shader based on shading mode setting
  shader.bind();

  // Update the uniform values. Note that it is better to only do this when the
  // matrices change, but for the sake of simplicity this was not done.

  shader.setUniformValueArray("modelViewTransforms", meshTransforms, MAX_MESH_COUNT);
  shader.setUniformValue("projectionTransform", projectionTransform);
  shader.setUniformValueArray("normalTransforms", normalTransforms, MAX_MESH_COUNT);

  // texture
  shader.setUniformValue("texSampler", 0); // = sample current "texture 0"

  // material uniforms
  shader.setUniformValue("matCol", QVector3D(1.0,1.0,1.0));
  shader.setUniformValue("matKA", (GLfloat)0.4);
  shader.setUniformValue("matKD", (GLfloat)0.4);
  shader.setUniformValue("matKS", (GLfloat)0.4);
  shader.setUniformValue("matExponent", (GLfloat)8.0);

  // current light position
  shader.setUniformValue("lightCol", QVector3D(1.0,1.0,1.0));
  shader.setUniformValue("lightPos", QVector3D(100.0,50.0,0.0));

  // bind and draw
  glBindVertexArray(meshVAO);
  glBindTexture(GL_TEXTURE_2D, meshTexture);
  glDrawArrays(GL_TRIANGLES, 0, meshSize);

  shader.release();
}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  Q_UNUSED(newWidth)
  Q_UNUSED(newHeight)
  updateProjectionTransform();
}

/**
 * @brief MainView::updateProjectionTransform Updates the projection transform
 * matrix taking into consideration the current aspect ratio.
 */
void MainView::updateProjectionTransform() {
  float aspectRatio =
      static_cast<float>(width()) / static_cast<float>(height());
  projectionTransform.setToIdentity();
  projectionTransform.perspective(60.0F, aspectRatio, 0.2F, 20.0F);
}

/**
 * @brief MainView::updateModelTransforms Updates the model transform matrix of
 * the mesh to reflect the current rotation and scale values.
 */
void MainView::updateModelTransforms() {
  for (int i = 0; i < meshCount; i++) {
    meshTransforms[i].setToIdentity();
    meshTransforms[i].translate(0.0F, 0.0F, -10.0F);
    meshTransforms[i].rotate(QQuaternion::fromEulerAngles(rotations[i]));
    meshTransforms[i].scale(scales[i]);
    normalTransforms[i] = meshTransforms[i].normalMatrix();
  }
  
  update();
}

/**
 * @brief MainView::destroyModelBuffers Cleans up the memory used by OpenGL.
 */
void MainView::destroyModelBuffers() {
  glDeleteBuffers(1, &meshPositionVBO);
  glDeleteBuffers(1, &meshNormalVBO);
  glDeleteBuffers(1, &meshTexCoordVBO);
  glDeleteBuffers(1, &meshIdVBO);
  glDeleteVertexArrays(1, &meshVAO);
}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
  rotations[0] = {static_cast<float>(rotateX), static_cast<float>(rotateY),
              static_cast<float>(rotateZ)};
  updateModelTransforms();
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
void MainView::setScale(float newScale) {
  scales[0] = newScale;
  updateModelTransforms();
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
