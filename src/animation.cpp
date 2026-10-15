#include "animation.h"

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "mainview.h"

/*
    animation file format as follows:
    -----
    MESHES
    <mesh0 filename>
    <mesh1 filename>
    ...
    <meshn filename>
    END
    SEQ
    <mesh0 kf0>
    <mesh0 kf1>
    ...
    <mesh0 kfn>
    END
    SEQ
    <...>
    END

    # comments start on a pound symbol
    ---------
    empty lines are ignored (skipped)
    keyframes are in the format:
    TIME POSX POSY POSZ ROTX ROTY ROTZ SCALEX SCALEY SCALEZ
*/
Animation::Animation(const QString &filename)
{
  bool meshesRead = false;
  bool reading = false;
  meshNames.reserve(MAX_MESH_COUNT);
  sequences = new std::vector<Keyframe>[MAX_MESH_COUNT];

  // default frame rate
  frameRate = 30;

  qDebug() << ":: Loading animation:" << filename;
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);
    QString line;
    QStringList args;
    int lineNum = 0;
    int currentMeshSeq = 0;

    while (!in.atEnd()) {
      line = in.readLine();
      lineNum++;
      // skip comments
      if (line.startsWith("#")) continue;
      args = line.split(" ", Qt::SkipEmptyParts);
      // skip empty lines also
      if (args.size() == 0) continue;

      if (!reading) {
        if (args[0] == "FRAMERATE") {
          if (args.size() < 2) {
            qDebug() << "SYNTAX ERROR: line" << lineNum << "- expected framerate";
          }
          frameRate = args[1].toInt();
          qDebug() << "Parsed framerate:" << frameRate;
          continue;
        }
      }

      if (!meshesRead && !reading) {
        if (args[0] != "MESHES") {
          qDebug() << "SYNTAX ERROR: line" << lineNum << "- expected start of mesh section";
          abort();
        }
        reading = true;
        continue;
      }
      if (meshesRead && !reading) {
        if (args[0] != "SEQ") {
          qDebug() << "SYNTAX ERROR: line" << lineNum << "- expected start of keyframe section or EOF";
          abort();
        }
        reading = true;
        continue;
      }
      if (reading) {
        if (args[0] == "END") {
          reading = false;
          // finish processing current section
          if (!meshesRead) {
            qDebug() << "Parsed" << meshNames.size() << "mesh name(s)";
            meshesRead = true;
          } else {
            qDebug() << "Parsed" << sequences[currentMeshSeq].size() << "keyframe(s) for seq" << currentMeshSeq;
            currentMeshSeq++;
          }
          continue;
        }
      }
      // add another element to currently-processed vector
      if (!meshesRead) {
        qDebug() << "Mesh" << meshNames.size() << "filename:" << args[0];
        meshNames.append(args[0]);
      } else {
        /*
        qDebug() << "KF time:" << args[0];
        qDebug() << "POS XYZ:" << args[1] << args[2] << args[3];
        qDebug() << "ROT XYZ:" << args[4] << args[5] << args[6];
        qDebug() << "SCL XYZ:" << args[7] << args[8] << args[9];
        */
        Keyframe keyframe(
          args[0].toInt(),
          QVector3D(args[1].toFloat(), args[2].toFloat(), args[3].toFloat()),
          QVector3D(args[4].toFloat(), args[5].toFloat(), args[6].toFloat()),
          QVector3D(args[7].toFloat(), args[8].toFloat(), args[9].toFloat())
        );
        sequences[currentMeshSeq].push_back(keyframe);
      }
    }

    file.close();
    qDebug() << "Processed" << lineNum << "lines";
  }
}

// TODO
Keyframe Animation::getInterpolatedFrame(const int meshId, const int time)
{
  // get sequence for mesh id
  std::vector<Keyframe> meshSequences = sequences[meshId];
  // find "previous (<=)" and "next (<=)" keyframe relative to current time
  // Keyframe previous, next;

  // find interpolation factor (linearly)
  // float factor; // [0;1]

  // interpolate keyframes according to factor
  // return Keyframe();
}

QVector<QString> Animation::getMeshFilenames()
{
    return meshNames;
}

float Animation::getFrameTime()
{
    return 1.0f / (float)frameRate;
}
