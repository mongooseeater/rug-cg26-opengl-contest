
# "The Animation-Viewer of Theseus"<br>RUG Computer Graphics 2026 OpenGL Contest submission

### Avery Kozák (s5559294)

This submission is an extension of the OpenGL "model viewer" framework from the second assignment. It's mostly about animation.

## Sample images

![](https://github.com/mongooseeater/rug-cg26-opengl-contest/raw/master/samplegifs/1cats.gif)

![](https://github.com/mongooseeater/rug-cg26-opengl-contest/raw/master/samplegifs/2solar.gif)

![](https://github.com/mongooseeater/rug-cg26-opengl-contest/raw/master/samplegifs/3saucer.gif)

![](https://github.com/mongooseeater/rug-cg26-opengl-contest/raw/master/samplegifs/4bunny.gif)

(For higher quality, see [here](#sample-animations) for how to view these inside the application.)

## Notable features

- **Multi-mesh support**. This is more of a consequence/necessary prerequisite for the animation system, but it's nevertheless present. You can put lots of cats next to each other.
- **Mesh-based animation support**. That is, animation of individual meshes (as opposed to part of a mesh via a "skeleton" and its "bones", i.e. armature-based animation.)
- **Keyframing and interpolation**. Animation files consist of "sequences" for each mesh, defined using a series of "keyframes" describing exact positions, rotations and scales at a given frame. Between two keyframes, the animation is interpolated.
	- Multiple interpolation modes are supported: constant ("none", i.e. no motion until next keyframe), linear, sine (in/out/both), quad (in/out/both) and bounce (it bounces)
- Animations can be played and paused, loaded on-demand and viewed per-frame. In fact, "animation files" are scene descriptions with sequence data. See [this section](#animation-file-format)

## Build instructions

The project is an extension of the OpenGL assignment framework, so it can be built in a similar manner.

The easiest way to do this is to open `src/CMakeLists.txt` in QtCreator and build+run it there.

The project doesn't have any special dependencies or requirements beyond those of the assignment framework, so whatever machine you worked on the assignments for will probably run this just fine, too.

## Usage

The application controls pretty much identically to the OpenGL assignment 2 framework, and the rest is self-explanatory.
- Rotation/scaling rotates/scales the entire scene around the origin, not the individual meshes.
- You can play/pause the animation using the corresponding button.
- You can view individual frames by setting "Frame #"

## Sample animations

Naturally, the project comes bundled with a few sample animations showcasing its capabilities. These can be found in the `animations` directory.

Animations can be loaded via the "Load animation..." button.

The models and the animations themselves were made in Blender by yours truly. Since the camera starts at scale 1.0 at the origin, you might have to rotate/scale these a bit to get a good view.


## Animation file format

The format for animation (.anim) files is ~~lazy~~ quite primitive so as to be easy to edit.

The sample files included are commented (particularly `test_cats`) so they hopefully shouldn't be too difficult to read.

In general, .anim files follow this structure:

    MESHES
    <mesh 0 file path>
    <mesh 1 file path>
    ...
    <mesh N file path>
    END

    DIFFUSETEX <diffuse texture path>
    FRAMERATE <frames per second>

    SEQ
    <mesh 0, frame 0 keyframe>
    <mesh 0, frame 1 keyframe>
    ...
    <mesh 0, frame L keyframe>
    END
    SEQ
    <mesh 1 keyframes>
    END
    ...
    SEQ
    <mesh N keyframes>
    END

Furthermore:
- Empty lines are ignored, as is (same-line) whitespace between elements.
- Lines beginning on `#` are **comments** and are also ignored.
- The **mesh section** (`MESHES ... END`) should be specified first, the **sequences** after.
- The mesh section should contain only mesh names and each sequence should contain only keyframes.
- **The order of mesh names and sequences matters!** Furthermore, each mesh should have exactly one sequence (however, this can just consist of a single keyframe at frame 0 if the object is stationary.)
- Other keys (DIFFUSETEX, FRAMERATE) can be added anywhere so long as it's not in the mesh section or inside a sequence.
- Specifying framerate is optional, it defaults to 30FPS. Value must be an integer.
- Specifying the diffuse texture is technically optional: of course, your meshes will be untextures and therefore all-black, which is probably not what you want, but it shouldn't crash.

The animation's "length" will be the length of its *longest sequence*. This is the interval of frames over which the animation will loop (with the caveat that the last frame is always *interpolated to, but never reached* when looping - this just makes it convenient to make looping animations.)

### Keyframe format

Each keyframe is specified on its own line in the format:


    FRAME_NUMBER INTERPOLATION_MODE POS_X POS_Y POS_Z ROT_X ROT_Y ROT_Z SCALE_X SCALE_Y SCALE_Z

Here, POS, ROT and SCALE are the position, rotation and scale in XYZ.

The numerical values for the interpolation modes are:
- 0: Constant (i.e. "none", simply keeps the previous keyframe)
- 1: Linear
- 2: Sine In-Out
- 3: Sine In (i.e. starts sinusoidal, "speeds up to" linear)
- 4: Sine Out (i.e. starts linear, "snow down to" sinusoidal)
- 5: Quadratic In-Out
- 6: Quadratic In
- 7: Quadratic Out
- 8: Bounce (oscillates a bit before settling)

### Blender: Keyframe conversion script

For the curious - and for the love of the game - also included is a small (and somewhat rudimentary) Python script `blender_converter.py` which you can use for converting an animation in Blender to the .anim format.

This is a somewhat awful, but significantly-less-horrible-than-manual-labour way of converting Blender animations to the .anim format.

For this to work, ensure that:
- (Transform) position, rotation and scale are all keyframed on the object, and nothing else is.
- Each keyframe contains all of these properties ("channels"). (An easy fix is to do "Key -> Insert keyframes -> All channels" for every keyframe.)
- For best results, ensure that the easing+interpolation match for each channel, and avoid using "automatic easing".

The script prints to Blender's *System Console* window (Window -> Toggle system console). 

You can copy the printed sequence (i.e. `SEQ ... END`) from the console window with CTRL-C and paste it into the .anim file at the right location (i.e. meshes and sequences must be in matching order).

## Attribution
Although the submission is individual, it is based on the shared Group 3 submission which I worked on along with **Hidde Nauta**. Kudos to him!

The individual submission was of course made with his permission.

Other than that, I guess don't feed this to an LLM and don't run around claiming it's yours or something like that.