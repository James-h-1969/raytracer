import bpy
import random
import math
from mathutils import Vector

def clean_scene():
    """
    Deletes all MESH objects from the scene and removes unused materials.
    """
    # Ensure we are in Object Mode
    if bpy.context.object and bpy.context.object.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')

    # Deselect all
    bpy.ops.object.select_all(action='DESELECT')

    # Select all mesh objects
    for obj in bpy.context.scene.objects:
        if obj.type == 'MESH':
            obj.select_set(True)

    # Delete selected
    bpy.ops.object.delete()

    # Clean up orphan materials (optional, keeps file size down)
    for block in bpy.data.materials:
        if block.users == 0:
            bpy.data.materials.remove(block)

def create_random_material():
    """
    Creates a new material with random color, metallic, and roughness values.
    """
    mat = bpy.data.materials.new(name=f"RandomMat_{random.randint(0, 1000)}")
    mat.use_nodes = True
    
    # Get the Principled BSDF node
    nodes = mat.node_tree.nodes
    bsdf = nodes.get("Principled BSDF")

    if bsdf:
        # Random Color (R, G, B, Alpha)
        color = (random.random(), random.random(), random.random(), 1.0)
        bsdf.inputs['Base Color'].default_value = color
        
        # Random Metallic (0.0 to 1.0)
        bsdf.inputs['Metallic'].default_value = random.random()
        
        # Random Roughness (0.0 to 1.0)
        bsdf.inputs['Roughness'].default_value = random.random()

    return mat

def get_random_camera_coord(camera, dist_min=5, dist_max=10, spread_x=5, spread_y=5):
    """
    Calculates a random world position relative to the camera's view.
    Updated: Objects are now placed further away (20-50 units).
    """
    # Random depth (Z is negative in camera local space for 'forward')
    z_depth = -random.uniform(dist_min, dist_max)
    
    # Random spread in X and Y (Left/Right, Up/Down relative to camera)
    # Increased spread slightly so they don't look clumped when further away
    x_spread = random.uniform(-spread_x, spread_x)
    y_spread = random.uniform(-spread_y, spread_y)
    
    # Create a vector in local camera space
    local_vec = Vector((x_spread, y_spread, z_depth))
    
    # Convert local camera space to world space using the camera's matrix
    world_vec = camera.matrix_world @ local_vec
    
    return world_vec

def generate_objects(count=30):
    """
    Generates random primitives in front of the active camera.
    """
    cam = bpy.context.scene.camera
    
    if not cam:
        print("No camera found! Please add a camera to the scene.")
        return

    # List of primitive adding functions (Restricted to Cubes, Spheres, Planes)
    primitives = [
        bpy.ops.mesh.primitive_cube_add,
        bpy.ops.mesh.primitive_uv_sphere_add,
        bpy.ops.mesh.primitive_plane_add
    ]

    for _ in range(count):
        # 1. Pick a random primitive type
        primitive_func = random.choice(primitives)
        
        # 2. Add the object (initially at 0,0,0)
        primitive_func()
        obj = bpy.context.active_object
        
        # 3. Move object to valid camera view position
        loc = get_random_camera_coord(cam)
        obj.location = loc
        
        # 4. Random Rotation
        obj.rotation_euler = (
            random.uniform(0, math.pi * 2),
            random.uniform(0, math.pi * 2),
            random.uniform(0, math.pi * 2)
        )
        
        # 5. Random Scale
        scale_factor = random.uniform(0.5, 2.0)
        obj.scale = (scale_factor, scale_factor, scale_factor)
        
        # 6. Apply Random Material
        mat = create_random_material()
        if obj.data.materials:
            obj.data.materials[0] = mat
        else:
            obj.data.materials.append(mat)
            
        # Shade smooth for spheres looks better (Planes/Cubes don't strictly need it but it doesn't hurt)
        bpy.ops.object.shade_smooth()

# --- Execution ---
if __name__ == "__main__":
    # 1. Clear existing meshes
    clean_scene()
    
    # 2. Generate new ones
    # Reduced count to 10
    generate_objects(count=10) 
    
    print("Generation complete.")