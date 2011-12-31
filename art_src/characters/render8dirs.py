import bpy
from math import radians

angle = 45
axis = 2 # z-axis
platform = bpy.data.objects["RenderPlatform"]
original_path = bpy.data.scenes[0].render.filepath

bpy.ops.render.view_show()

for i in range(0,8):
	
	# rotate the render platform and all children
	temp_rot = platform.rotation_euler
	temp_rot[axis] = temp_rot[axis] - radians(angle)
	platform.rotation_euler = temp_rot;
	
	# set the filename direction prefix
	bpy.data.scenes[0].render.filepath = original_path + str(i)
	
	# render animation for this direction
	bpy.ops.render.render(animation=True)

bpy.data.scenes[0].render.filepath = original_path
