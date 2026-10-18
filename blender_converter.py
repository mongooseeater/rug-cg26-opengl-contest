# =========================================================
# Set this variable to the name in the inspector of your object (i.e. the one whose keyframes you want):

obj_name = 'object_name_here'

# ================ SCRIPT BEGINS HERE =====================

import bpy
from bpy_extras import anim_utils
import decimal
from bpy import context

def interp_value(interp, easing):
    if interp == 'CONSTANT':
        return 0
    if interp == 'LINEAR':
        return 1
    if interp == 'BOUNCE':
        return 8
    if easing == 'AUTO':
        print("Warning: Easing set to AUTO on some keyframes: no promises!")
    if interp == 'SINE':
        if easing == 'AUTO' or easing == 'EASE_IN_OUT':
            return 2
        if easing == 'EASE_OUT':
            return 3
        if easing == 'EASE_IN':
            return 4
    if interp == 'QUAD':
        if easing == 'AUTO' or easing == 'EASE_IN_OUT':
            return 5
        if easing == 'EASE_OUT':
            return 6
        if easing == 'EASE_IN':
            return 7
    
    print("WARNING: Unsupported easing/interpolation type. Defaulting to linear.")
    return 1

anim_data = bpy.data.objects[obj_name].animation_data
channelbag = anim_utils.action_get_channelbag_for_slot(anim_data.action, anim_data.action_slot)

print("")
print("Parsing animations for " + obj_name + " ...")

# get frame data

time = []
for keyframe in channelbag.fcurves[0].keyframe_points:
    time.append(int(keyframe.co[0]))

# swap Y/Z
indices = [0, 2, 1, 3, 5, 4, 6, 8, 7]
rotation_indices = [3, 5, 4]

for i in indices:
    fcu = channelbag.fcurves[i]

lines = []
for tinx in range(len(time)):
    line = '';
    # frame (time)
    line = line + str(time[tinx]) + " "
    
    # interpolation
    interp = channelbag.fcurves[i].keyframe_points[tinx].interpolation
    easing = channelbag.fcurves[i].keyframe_points[tinx].easing
    line = line + str(interp_value(interp, easing)) + " "
    
    # pos/rot/scale
    for i in indices:
        val = channelbag.fcurves[i].keyframe_points[tinx].co[1]
        # radians -> degrees
        if i in rotation_indices:
            val = (val / (3.1415926535)) * -180.0
        line = line + str(round(val, 3)) + " "
    lines.append(line)
print('Done')
print('Parsed sequence:\n')
print('# ' + obj_name)
print('SEQ')
for line in lines:
    print(line)
print('END')