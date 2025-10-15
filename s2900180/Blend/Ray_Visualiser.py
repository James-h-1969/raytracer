import bpy
import mathutils

# ===============================================================
# CONFIGURATION
# ===============================================================
RAYS_FILE_PATH = "/home/jhocking542/computer-graphics/raytracer/s2900180/Output/rays.txt"  # <-- Change this path
CYLINDER_RADIUS = 0.001
CYLINDER_SEGMENTS = 12
ARROW_SIZE = 0.05
RAY_LENGTH = 2.0  # Default scaling factor if direction vectors are normalized

# ===============================================================
# HELPER FUNCTIONS
# ===============================================================
def create_ray(origin, direction, index):
    """
    Create a cylinder and arrowhead to represent a ray.
    """
    origin_vec = mathutils.Vector(origin)
    direction_vec = mathutils.Vector(direction).normalized() * RAY_LENGTH

    # Compute the end point
    end_point = origin_vec + direction_vec

    # Create the cylinder (the ray body)
    bpy.ops.mesh.primitive_cylinder_add(
        radius=CYLINDER_RADIUS,
        depth=(origin_vec - end_point).length,
        location=(origin_vec + end_point) / 2.0,
        vertices=CYLINDER_SEGMENTS
    )
    cylinder = bpy.context.object
    cylinder.name = f"Ray_{index}_Body"

    # Align cylinder to the ray direction
    cylinder_direction = end_point - origin_vec
    cylinder_direction.normalize()
    up = mathutils.Vector((0, 0, 1))
    rot_axis = up.cross(cylinder_direction)
    if rot_axis.length > 0:
        rot_axis.normalize()
        angle = up.angle(cylinder_direction)
        cylinder.rotation_mode = 'AXIS_ANGLE'
        cylinder.rotation_axis_angle = (angle, rot_axis.x, rot_axis.y, rot_axis.z)

    return cylinder


def clear_previous_rays():
    """
    Delete all previously created objects named Ray_*.
    """
    bpy.ops.object.select_all(action='DESELECT')
    for obj in bpy.data.objects:
        if obj.name.startswith("Ray_"):
            obj.select_set(True)
    bpy.ops.object.delete()


# ===============================================================
# MAIN SCRIPT
# ===============================================================
def read_rays_from_file(filepath):
    """
    Expected format (one ray per line):
    origin_x origin_y origin_z dir_x dir_y dir_z
    """
    rays = []
    with open(filepath, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) != 6:
                continue
            origin = tuple(map(float, parts[:3]))
            direction = tuple(map(float, parts[3:]))
            rays.append((origin, direction))
    return rays


def visualize_rays(filepath):
    clear_previous_rays()
    rays = read_rays_from_file(filepath)
    print(f"Loaded {len(rays)} rays from file.")

    for i, (origin, direction) in enumerate(rays): 
        create_ray(origin, direction, i)


# ===============================================================
# RUN
# ===============================================================
if __name__ == "__main__":
    visualize_rays(RAYS_FILE_PATH)
