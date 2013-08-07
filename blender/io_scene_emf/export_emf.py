
import os
import time

import bpy
import mathutils
import bpy_extras.io_utils

"""
bl_info = {
	"name": "Export Physics Shapes (.yaml)",
	"author": "Stuart Denman",
	"version": (1, 0),
	"blender": (2, 5, 7),
#	"api": 35622,
	"location": "File > Export",
	"description": "Export physics shapes under the selected empty named 'physics'.",
	"warning": "",
	"wiki_url": "",
	"tracker_url": "",
	"category": "Import-Export"}

'''
Usage Notes:
To create a compound physics collision shape for a mesh in blender:

1. place the 3D cursor at the origin of the mesh object.
2. Add > Empty, name it "physics"
3. Create a physics shape with Add > Mesh > Cube, UV Sphere, Cylinder, Cone or create an arbitrary mesh for a ConvexHull shape.
4. Parent the new shape to the "physics" Empty.
5. The mesh name must start with: Box, Sphere, Cylinder, Cone, Capsule, or ConvexHull, depending on the shape you want.
6. Position and scale the shape object, but do not modify the internal vertices, unless it is a ConvexHull type.
7. Repeat step 3-6 until your shape is complete. Shapes can only be a 1-level deep hierarchy.
8. IMPORTANT: Select the empty object you named "physics"
9. Click File > Export > Physics Shapes (.yaml)
'''

import bpy
from bpy.props import *
import mathutils, math, struct
from mathutils import *
import os
from os import remove
import time
import bpy_extras
from bpy_extras.io_utils import ExportHelper 
import time
import shutil


# Methods for writing point, scale, and quaternion types to a YAML file.
# This particular implementation converts values to a Y-up coordinate system.
def out_point3_y_up( v ):
	return "[%g,%g,%g]" % ( v.x, v.z, -v.y )
def out_scale3_y_up( s ):
	return "[%g,%g,%g]" % ( s.x, s.z, s.y )
def out_quaternion_y_up( q ):
	return "[%g,%g,%g,%g]" % ( q.w, q.x, q.z, -q.y )
# This implementation maintains blender's Z-up coordinate system.
def out_point3_z_up( v ):
	return "[%g,%g,%g]" % ( v.x, v.y, v.z )
def out_scale3_z_up( s ):
	return "[%g,%g,%g]" % ( s.x, s.y, s.z )
def out_quaternion_z_up( q ):
	return "[%g,%g,%g,%g]" % ( q.w, q.x, q.y, q.z )



def getPhysicsShape( obj, use_y_up ):
	shape = ""
	props = { }
	name = obj.name.lower()
	scale = Vector(( abs(obj.scale.x), abs(obj.scale.y), abs(obj.scale.z) ))

	if use_y_up:
		out_point3 = out_point3_y_up
		out_scale3 = out_scale3_y_up
		out_quaternion = out_quaternion_y_up
	else:
		out_point3 = out_point3_z_up
		out_scale3 = out_scale3_z_up
		out_quaternion = out_quaternion_z_up	
	
	# BOX
	if name.startswith('box'):
		shape = "Box"
		props["half-extents"] = out_scale3( scale )
	# SPHERE
	elif name.startswith('sph'):
		shape = "Sphere"
		props["radius"] = obj.scale.x
	# CONE
	elif name.startswith('cone'):
		shape = "Cone"
		props["radius"] = obj.scale.x
		props["height"] = obj.scale.z * 2.0
	# CYLINDER
	elif name.startswith('cyl'):
		shape = "Cylinder"
		props["half-extents"] = out_scale3( scale )
	# CAPSULE
	elif name.startswith('cap'):
		shape = "Capsule"
		props["radius"] = obj.scale.x
		props["height"] = obj.scale.z
	# CONVEX-HULL
	elif name.startswith('convex'):
		shape = "ConvexHull"
		mesh = obj.to_mesh( bpy.context.scene, True, 'PREVIEW' )
		props["points"] = "\n"
		for v in mesh.vertices:
			props["points"] += "    - " + out_point3( v.co ) + "\n"
		props["points"] = props["points"].rstrip("\n")
		if scale != Vector((1,1,1)):
			props["scale"] = out_scale3( scale )
		# remove mesh
	
	if obj.location != Vector((0,0,0)):
		props["origin"] = out_point3( obj.location )
	if obj.rotation_mode == 'QUATERNION':
		qrot = obj.rotation_quaternion
	else:
		qrot = obj.matrix_local.to_quaternion()
	if qrot != Quaternion((1,0,0,0)):
		props["rotate"] = out_quaternion( qrot )
	
	return (shape, props)


###### EXPORT OPERATOR #######
class ExportPhysicsShape(bpy.types.Operator, ExportHelper):
	'''Export physics shapes under the selected empty named "physics".'''
	bl_idname = "object.export_physics"
	bl_label = "Export Physics Shapes"
	
	filename_file = ""
	filename_ext = ".yaml"
	
	filter_glob = StringProperty(default="*.yaml", options={'HIDDEN'})
	
	append_to_existing = BoolProperty(name="Append To Existing File",
							description="Appends the physics shapes to an existing file",
							default=True)
	
	use_y_up = BoolProperty(name="Convert To Y-Up",
							description="Converts the values to a Y-Axis Up coordinate system",
							default=True)

	object_name = StringProperty(name="Root Name",
							description="The top-level name that will contain the physics shapes",
							default="StaticMesh/object" )


	@classmethod
	def poll(cls, context):
		return context.active_object.type in ['EMPTY'] and context.active_object.name == 'physics'

	def execute(self, context):
		filepath = self.filepath
		filepath = bpy.path.ensure_ext(filepath, self.filename_ext)
		ob_base = context.active_object

		out = open( filepath, ["w","a"][self.append_to_existing] )
		out.write( self.object_name + ":\n" )
		out.write( "  physics:\n" )
		
		for c in ob_base.children:
			if c.type != 'MESH':
				continue
			(shape, props) = getPhysicsShape( c, self.use_y_up )
			out.write( "  - shape: " + shape + "\n" )
			for (k,v) in props.items():
				out.write( "    %s: %s\n" % (k, v) )
				
		out.close();	
		self.report( {'INFO'}, "Export succeeded!" )
		return {'FINISHED'}

	def invoke(self, context, event):
		wm = context.window_manager

		# change the default object name to use the current .blend filename
		name = bpy.path.display_name_from_filepath(bpy.data.filepath)
		self.object_name = "StaticMesh/" + name
		self.filepath = name

		if True:
			# File selector
			wm.fileselect_add(self) # will run self.execute()
			return {'RUNNING_MODAL'}
		elif True:
			# search the enum
			wm.invoke_search_popup(self)
			return {'RUNNING_MODAL'}
		elif False:
			# Redo popup
			return wm.invoke_props_popup(self, event) #
		elif False:
			return self.execute(context)

"""


def getPhysicsShape(obj):
	name = obj.name.lower()
	# BOX
	if name.startswith('box'):
		shape = "Box"
	# SPHERE
	elif name.startswith('sph'):
		shape = "Sphere"
	# CONE
	elif name.startswith('cone'):
		shape = "Cone"
	# CYLINDER
	elif name.startswith('cyl'):
		shape = "Cylinder"
	# CAPSULE
	elif name.startswith('cap'):
		shape = "Capsule"
	# CONVEX-HULL
	elif name.startswith('convex'):
		shape = "ConvexHull"
	
	print("find a '%s'" % shape)

def writeCollisionShape(object, file, collisionName):
	print("need write collision Shape ... \n")
	fw = file.write
	fw('\nCollisions shapes :\n')
	fw('\t\n')
	for c in object.children:
		if c.type != 'MESH':
			continue
		getPhysicsShape( c )



def name_compat(name):
	if name is None:
		return 'None'
	else:
		return name.replace(' ', '_')


def mesh_triangulate(me):
	import bmesh
	bm = bmesh.new()
	bm.from_mesh(me)
	bmesh.ops.triangulate(bm, faces=bm.faces)
	bm.to_mesh(me)
	bm.free()


def write_mtl(scene, file, filepath, path_mode, copy_set, mtl_dict):
	from mathutils import Color
	world = scene.world
	if world:
		world_amb = world.ambient_color
	else:
		world_amb = Color((0.0, 0.0, 0.0))
	source_dir = os.path.dirname(bpy.data.filepath)
	dest_dir = os.path.dirname(filepath)
	fw = file.write
	fw('\nMaterials : %i\n' % len(mtl_dict))
	mtl_dict_values = list(mtl_dict.values())
	mtl_dict_values.sort(key=lambda m: m[0])
	# Write material/image combinations we have used.
	# Using mtl_dict.values() directly gives un-predictable order.
	for mtl_mat_name, mat, face_img in mtl_dict_values:
		# Get the Blender data for the material and the image.
		# Having an image named None will make a bug, dont do it :)
		fw('\t%s\n' % mtl_mat_name)  # Define a new material: matname_imgname
		if mat:
			# convert from blenders spec to 0 - 1000 range.
			if mat.specular_shader == 'WARDISO':
				tspec = (0.4 - mat.specular_slope) / 0.0004
			else:
				tspec = (mat.specular_hardness - 1) * 1.9607843137254901
			fw('\t\tNs %.6f\n' % tspec)
			del tspec
			fw('\t\tKa %.6f %.6f %.6f\n' % (mat.ambient * world_amb)[:])  # Ambient, uses mirror color,
			fw('\t\tKd %.6f %.6f %.6f\n' % (mat.diffuse_intensity * mat.diffuse_color)[:])  # Diffuse
			fw('\t\tKs %.6f %.6f %.6f\n' % (mat.specular_intensity * mat.specular_color)[:])  # Specular
			if hasattr(mat, "ior"):
				fw('\t\tNi %.6f\n' % mat.ior)  # Refraction index
			else:
				fw('\t\tNi %.6f\n' % 1.0)
			fw('\t\td %.6f\n' % mat.alpha)  # Alpha (obj uses 'd' for dissolve)
			# 0 to disable lighting, 1 for ambient & diffuse only (specular color set to black), 2 for full lighting.
			if mat.use_shadeless:
				fw('\t\tillum 0\n')  # ignore lighting
			elif mat.specular_intensity == 0:
				fw('\t\tillum 1\n')  # no specular.
			else:
				fw('\t\tillum 2\n')  # light normaly
		else:
			#write a dummy material here?
			fw('\t\tNs 0\n')
			fw('\t\tKa %.6f %.6f %.6f\n' % world_amb[:])  # Ambient, uses mirror color,
			fw('\t\tKd 0.8 0.8 0.8\n')
			fw('\t\tKs 0.8 0.8 0.8\n')
			fw('\t\td 1\n')  # No alpha
			fw('\t\tillum 2\n')  # light normaly
		# Write images!
		if face_img:  # We have an image on the face!
			filepath = face_img.filepath
			if filepath:  # may be '' for generated images
				# write relative image path
				filepath = bpy_extras.io_utils.path_reference(filepath,
				                                              source_dir,
				                                              dest_dir,
				                                              path_mode,
				                                              "",
				                                              copy_set,
				                                              face_img.library)
				fw('\t\tmap_Kd %s\n' % filepath)  # Diffuse mapping image
				del filepath
			else:
				# so we write the materials image.
				face_img = None
		if mat:  # No face image. if we havea material search for MTex image.
			image_map = {}
			# backwards so topmost are highest priority
			for mtex in reversed(mat.texture_slots):
				if mtex and mtex.texture and mtex.texture.type == 'IMAGE':
					image = mtex.texture.image
					if image:
						# texface overrides others
						if(     mtex.use_map_color_diffuse
						    and (face_img is None)
						    and (mtex.use_map_warp is False)
						    and (mtex.texture_coords != 'REFLECTION')
						   ):
							image_map["map_Kd"] = image
						if mtex.use_map_ambient:
							image_map["map_Ka"] = image
						# this is the Spec intensity channel but Ks stands for specular Color
						if mtex.use_map_color_spec:  # specular color
							image_map["map_Ks"] = image
						if mtex.use_map_hardness:  # specular hardness/glossiness
							image_map["map_Ns"] = image
						if mtex.use_map_alpha:
							image_map["map_d"] = image
						if mtex.use_map_translucency:
							image_map["map_Tr"] = image
						if mtex.use_map_normal and (mtex.texture.use_normal_map is True):
							image_map["map_Bump"] = image
						if mtex.use_map_normal and (mtex.texture.use_normal_map is False):
							image_map["map_Disp"] = image
						if mtex.use_map_color_diffuse and (mtex.texture_coords == 'REFLECTION'):
							image_map["map_refl"] = image
						if mtex.use_map_emit:
							image_map["map_Ke"] = image
			for key, image in image_map.items():
				filepath = bpy_extras.io_utils.path_reference(image.filepath,
				                                              source_dir,
				                                              dest_dir,
				                                              path_mode,
				                                              "",
				                                              copy_set,
				                                              image.library)
				fw('\t\t%s %s\n' % (key, repr(filepath)[1:-1]))


"""
 " @brief Basic write function. The context and options must be already set.
"""
def write_file(filepath,
               objects,
               scene,
               EXPORT_GLOBAL_MATRIX=None,
               EXPORT_PATH_MODE='AUTO',
               EXPORT_BINARY_MODE=False,
               EXPORT_COLLISION_NAME="",
               ):
	if EXPORT_GLOBAL_MATRIX is None:
		EXPORT_GLOBAL_MATRIX = mathutils.Matrix()
	
	def veckey3d(v):
		return round(v.x, 6), round(v.y, 6), round(v.z, 6)
	
	def veckey2d(v):
		return round(v[0], 6), round(v[1], 6)
	
	print('EMF Export path: %r' % filepath)
	
	time1 = time.time()
	
	mtlfilepath = os.path.splitext(filepath)[0] + ".mtl"
	
	file = open(filepath, "w", encoding="utf8", newline="\n")
	fw = file.write
	
	# Write Header
	fw('EMF(STRING)\n') # if binary : fw('EMF(BINARY)\n')
	fw('# Blender v%s EMF File: %r\n' % (bpy.app.version_string, os.path.basename(bpy.data.filepath)))
	
	# Initialize totals, these are updated each object
	totverts = totuvco = totno = 1
	
	face_vert_index = 1
	
	globalNormals = {}
	
	# A Dict of Materials
	# (material.name, image.name):matname_imagename # matname_imagename has gaps removed.
	mtl_dict = {}
	# Used to reduce the usage of matname_texname materials, which can become annoying in case of
	# repeated exports/imports, yet keeping unique mat names per keys!
	# mtl_name: (material.name, image.name)
	mtl_rev_dict = {}
	
	copy_set = set()
	
	# Get all meshes
	for ob_main in objects:
		print("object : '%s'" % str(ob_main))
		#print("name : '%s'" % ob_main.name)
		#for plop in ob_main.child:
		#	print("    child : '%s'" % plop.name)
		# ignore dupli children
		if ob_main.parent and ob_main.parent.dupli_type in {'VERTS', 'FACES'}:
			# XXX
			print(ob_main.name, 'is a dupli child - ignoring')
			continue
		obs = []
		if ob_main.dupli_type != 'NONE':
			# XXX
			print('creating dupli_list on', ob_main.name)
			ob_main.dupli_list_create(scene)
			obs = [(dob.object, dob.matrix) for dob in ob_main.dupli_list]
			# XXX debug print
			print(ob_main.name, 'has', len(obs), 'dupli children')
		else:
			obs = [(ob_main, ob_main.matrix_world)]
		idMesh=0
		for ob, ob_mat in obs:
			try:
				# apply the mesh modifieur at the curent object :
				me = ob.to_mesh(scene, True, 'PREVIEW', calc_tessface=False)
			except RuntimeError:
				me = None
			if me is None:
				continue
			idMesh = idMesh+1;
			fw('Mesh : %d\n' % idMesh)
			me.transform(EXPORT_GLOBAL_MATRIX * ob_mat)
			# _must_ do this first since it re-allocs arrays
			# triangulate all the mesh :
			mesh_triangulate(me)
			# export UV mapping :
			faceuv = len(me.uv_textures) > 0
			if faceuv:
				uv_texture = me.uv_textures.active.data[:]
				uv_layer = me.uv_layers.active.data[:]
			me_verts = me.vertices[:]
			# Make our own list so it can be sorted to reduce context switching
			face_index_pairs = [(face, index) for index, face in enumerate(me.polygons)]
			# faces = [ f for f in me.tessfaces ]
			edges = me.edges
			if not (len(face_index_pairs) + len(edges) + len(me.vertices)):  # Make sure there is somthing to write
				# clean up
				bpy.data.meshes.remove(me)
				continue  # dont bother with this mesh.
			# calculated normals:
			me.calc_normals()
			materials = me.materials[:]
			material_names = [m.name if m else None for m in materials]
			# avoid bad index errors
			if not materials:
				materials = [None]
				material_names = [name_compat(None)]
			# Sort by Material, then images
			# so we dont over context switch in the obj file.
			if faceuv:
				face_index_pairs.sort(key=lambda a: (a[0].material_index, hash(uv_texture[a[1]].image), a[0].use_smooth))
			elif len(materials) > 1:
				face_index_pairs.sort(key=lambda a: (a[0].material_index, a[0].use_smooth))
			else:
				# no materials
				face_index_pairs.sort(key=lambda a: a[0].use_smooth)
			# Set the default mat to no material and no image.
			contextMat = 0, 0  # Can never be this, so we will label a new material the first chance we get.
			contextSmooth = None  # Will either be true or false,  set bad to force initialization switch.
			# use : blen obs ??? what is this ....
			if True:
				name1 = ob.name
				name2 = ob.data.name
				if name1 == name2:
					obnamestring = name_compat(name1)
				else:
					obnamestring = '%s_%s' % (name_compat(name1), name_compat(name2))
				fw('\tName : %s\n' % obnamestring)  # Write Object name
			###########################################################
			## Vert
			###########################################################
			fw('\tVertex : %d\n\t\t' % len(me_verts))
			for v in me_verts:
				fw('%.6f %.6f %.6f|' % v.co[:])
			fw('\n')
			###########################################################
			## UV
			###########################################################
			fw('\tUV-mapping :\n\t\t')
			if faceuv:
				# in case removing some of these dont get defined.
				uv = uvkey = uv_dict = f_index = uv_index = None
				uv_face_mapping = [None] * len(face_index_pairs)
				uv_dict = {}  # could use a set() here
				for f, f_index in face_index_pairs:
					uv_ls = uv_face_mapping[f_index] = []
					for uv_index, l_index in enumerate(f.loop_indices):
						uv = uv_layer[l_index].uv
						uvkey = veckey2d(uv)
						try:
							uv_k = uv_dict[uvkey]
						except:
							uv_k = uv_dict[uvkey] = len(uv_dict)
							fw('%.6f %.6f|' % uv[:])
						uv_ls.append(uv_k)
				uv_unique_count = len(uv_dict)
				del uv, uvkey, uv_dict, f_index, uv_index, uv_ls, uv_k
				# Only need uv_unique_count and uv_face_mapping
			fw('\n')
			###########################################################
			## NORMAL
			###########################################################
			if f.use_smooth:
				localIsSmooth = 'vertex'
			else:
				localIsSmooth = 'face'
			fw('\tNormal(%s) : %d\n\t\t' % (localIsSmooth, len(face_index_pairs)) )
			for f, f_index in face_index_pairs:
				if f.use_smooth:
					for v_idx in f.vertices:
						v = me_verts[v_idx]
						noKey = veckey3d(v.normal)
						if noKey not in globalNormals:
							globalNormals[noKey] = totno
							totno += 1
							fw('%.6f %.6f %.6f|' % noKey)
				else:
					# Hard, 1 normal from the face.
					noKey = veckey3d(f.normal)
					if noKey not in globalNormals:
						globalNormals[noKey] = totno
						totno += 1
						fw('%.6f %.6f %.6f|' % noKey)
			
			fw('\n')
			if not faceuv:
				f_image = None
			###########################################################
			## faces
			###########################################################
			fw('\tFace : %d' % len(face_index_pairs))
			for f, f_index in face_index_pairs:
				f_smooth = f.use_smooth
				f_mat = min(f.material_index, len(materials) - 1)
				if faceuv:
					tface = uv_texture[f_index]
					f_image = tface.image
				# MAKE KEY
				if faceuv and f_image:  # Object is always true.
					key = material_names[f_mat], f_image.name
				else:
					key = material_names[f_mat], None  # No image, use None instead.
				# CHECK FOR CONTEXT SWITCH
				if key == contextMat:
					pass  # Context already switched, dont do anything
				else:
					if key[0] is None and key[1] is None:
						# inform the use of a material :
						fw("\n\t\t---:")  # mat, image
					else:
						mat_data = mtl_dict.get(key)
						if not mat_data:
							# First add to global dict so we can export to mtl
							# Then write mtl
							# Make a new names from the mat and image name,
							# converting any spaces to underscores with name_compat.
							# If none image dont bother adding it to the name
							# Try to avoid as much as possible adding texname (or other things)
							# to the mtl name (see [#32102])...
							mtl_name = "%s" % name_compat(key[0])
							if mtl_rev_dict.get(mtl_name, None) not in {key, None}:
								if key[1] is None:
									tmp_ext = "_NONE"
								else:
									tmp_ext = "_%s" % name_compat(key[1])
								i = 0
								while mtl_rev_dict.get(mtl_name + tmp_ext, None) not in {key, None}:
									i += 1
									tmp_ext = "_%3d" % i
								mtl_name += tmp_ext
							mat_data = mtl_dict[key] = mtl_name, materials[f_mat], f_image
							mtl_rev_dict[mtl_name] = key
						# set the use of a material :
						fw("\n\t\t%s:" % mat_data[0])  # can be mat_image or (null)
				contextMat = key
				f_v = [(vi, me_verts[v_idx]) for vi, v_idx in enumerate(f.vertices)]
				if faceuv:
					# export the normals :
					if f_smooth:  # Smoothed, use vertex normals
						for vi, v in f_v:
							fw(" %d/%d/%d" %
							   (v.index + totverts-1,
							    totuvco + uv_face_mapping[f_index][vi]-1,
							    globalNormals[veckey3d(v.normal)]-1,
							   ))  # vert, uv, normal
					else:  # No smoothing, face normals
						no = globalNormals[veckey3d(f.normal)]
						for vi, v in f_v:
							fw(" %d/%d/%d" %
							   (v.index + totverts-1,
							    totuvco + uv_face_mapping[f_index][vi]-1,
							    no-1,
							   ))  # vert, uv, normal
					face_vert_index += len(f_v)
				else:  # No UV's
					# export the normals :
					if f_smooth:  # Smoothed, use vertex normals
						for vi, v in f_v:
							fw(" %d//%d" % (
									   v.index + totverts-1,
									   globalNormals[veckey3d(v.normal)]-1,
									   ))
					else:  # No smoothing, face normals
						no = globalNormals[veckey3d(f.normal)]
						for vi, v in f_v:
							fw(" %d//%d" % (v.index + totverts-1, no-1))
				fw('|')
			fw('\n')
			# Write edges. ==> did not know what it is ...
			#fw('Faces : %d' % len(edges))
			#for ed in edges:
			#	if ed.is_loose:
			#		fw('%d %d\n' % (ed.vertices[0] + totverts, ed.vertices[1] + totverts))
			
			# Make the indices global rather then per mesh
			totverts += len(me_verts)
			if faceuv:
				totuvco += uv_unique_count
			# clean up
			bpy.data.meshes.remove(me)
		if ob_main.dupli_type != 'NONE':
			ob_main.dupli_list_clear()
	
	#####################################################################
	## Now we have all our materials, save them in the material section
	#####################################################################
	write_mtl(scene, file, mtlfilepath, EXPORT_PATH_MODE, copy_set, mtl_dict)
	#####################################################################
	## Save collision shapes :
	#####################################################################
	#	writeCollisionShape(ob_main, file, EXPORT_COLLISION_NAME)
	writeCollisionShape(objects, file, EXPORT_COLLISION_NAME)
	
	#####################################################################
	## End of the file generation:
	#####################################################################
	file.close()
	
	# copy all collected files.
	bpy_extras.io_utils.path_reference_copy(copy_set)
	
	print("EMF Export time: %.2f" % (time.time() - time1))

def getChildren(obj, allObj):
	children = []
	for ob in allObj:
		if ob.parent == obj:
			children.append(ob)
	if len(children) != 0:
		return children
	else:
		return None

"""
 " @brief generate the requested object file ... with his material inside and ...
 " 
"""
def _write(context,
           filepath,
           EXPORT_SEL_ONLY,
           EXPORT_GLOBAL_MATRIX,
           EXPORT_PATH_MODE,
           EXPORT_BINARY_MODE,
           EXPORT_COLLISION_NAME,
           ):
	# 
	base_name, ext = os.path.splitext(filepath)
	# create the output name : 
	context_name = [base_name, '', '', ext]  # Base name, scene name, frame number, extension
	# get the curent scene :
	scene = context.scene
	# Exit edit mode before exporting, so current object states are exported properly.
	if bpy.ops.object.mode_set.poll():
		bpy.ops.object.mode_set(mode='OBJECT')
	# get the curent frame selected :
	frame = scene.frame_current
	# Loop through all frames in the scene and export.
	scene.frame_set(frame, 0.0)
	# get only the object that are selected or all ...
	if EXPORT_SEL_ONLY:
		objects = context.selected_objects
	else:
		objects = scene.objects
	
	full_path = ''.join(context_name)
	
	for objj in objects:
		print("Object : '%s'" % (objj.name))
		for subObj in getChildren(objj, scene.objects):
			print("     find subObject : '%s'" % (subObj.name))
	
	#print("-----------------------------------------------------------------------")
	#print("Display object game property : ")
	#for objj in objects:
	#	print("    Object : '%s' : %s" % (objj.name, str(objj.game)))
	#	for element
	#	print("         actor : '%s'" % str(objj.game.use_actor))
	#	print("         type : '%s'" % str(objj.game.collision_bounds_type))
	#	print("         physique : '%s'" % str(objj.game.physics_type))
	#print("-----------------------------------------------------------------------")
	#print("plop:%s" % (str(dir(bpy.data.objects["Cube"].game)))
	#result : plop:['__doc__', '__module__', '__slots__', 'actuators', 'bl_rna', 'collision_bounds_type', 'collision_group', 'collision_margin', 'collision_mask', 'controllers', 'damping', 'fall_speed', 'form_factor', 'friction_coefficients', 'jump_speed', 'lock_location_x', 'lock_location_y', 'lock_location_z', 'lock_rotation_x', 'lock_rotation_y', 'lock_rotation_z', 'mass', 'obstacle_radius', 'physics_type', 'properties', 'radius', 'rna_type', 'rotation_damping', 'sensors', 'show_actuators', 'show_controllers', 'show_debug_state', 'show_sensors', 'show_state_panel', 'soft_body', 'states_initial', 'states_visible', 'step_height', 'use_activity_culling', 'use_actor', 'use_all_states', 'use_anisotropic_friction', 'use_collision_bounds', 'use_collision_compound', 'use_ghost', 'use_material_physics_fh', 'use_obstacle_create', 'use_rotate_from_normal', 'use_sleep', 'used_states', 'velocity_max', 'velocity_min']
	
	write_file(full_path,
	           objects,
	           scene,
	           EXPORT_GLOBAL_MATRIX,
	           EXPORT_PATH_MODE,
	           EXPORT_BINARY_MODE,
	           EXPORT_COLLISION_NAME,
	           )
	


"""
 " @brief Save the current element in the file requested.
 " 
"""
def save(operator,
         context,
         filepath="",
         use_selection=True,
         use_binary=False,
         collision_object_name="",
         global_matrix=None,
         path_mode='AUTO'
         ):
	_write(context,
	       filepath,
	       EXPORT_SEL_ONLY=use_selection,
	       EXPORT_GLOBAL_MATRIX=global_matrix,
	       EXPORT_PATH_MODE=path_mode,
	       EXPORT_BINARY_MODE=use_binary,
	       EXPORT_COLLISION_NAME=collision_object_name,
	       )
	
	return {'FINISHED'}
