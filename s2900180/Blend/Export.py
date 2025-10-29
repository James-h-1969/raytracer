# Export.py 
# This script takes a .blend scene and processes all the 
# import objects, creating a .json object with all lights,
# camera and object details.
#
# James Hocking 2025, assisted w. ChatGPT

import bpy
import json
import mathutils
import math

### PARAMETERS: Only change here 
OUTPUT_FILE_PATH = "/home/jhocking542/computer-graphics/raytracer/s2900180/ASCII/sphere_skew.json"
################################

def vector_to_list(v):
    """Convert a Blender Vector to a plain Python list."""
    return [float(v.x), float(v.y), float(v.z)]

def get_camera_gaze_vector(camera_obj):
    """Return the world-space gaze vector of a camera (-Z in local coordinates)."""
    local_forward = mathutils.Vector((0.0, 0.0, -1.0))
    world_forward = camera_obj.matrix_world.to_3x3() @ local_forward
    return world_forward.normalized()

def get_mesh_type(obj):
    """Heuristic classification of mesh types."""
    mesh = obj.data
    verts = len(mesh.vertices)
    faces = len(mesh.polygons)
    if verts == 4 and faces == 1:
        return "PLANE"
    elif verts == 8 and faces == 6:
        return "CUBE"
    elif verts > 12:
        return "SPHERE"
    return "OTHER"

def get_mesh_data(obj):
    """Extract geometric info depending on mesh type."""
    mesh_type = get_mesh_type(obj)
    if mesh_type == "SPHERE":
        # Get transform components
        loc = obj.matrix_world.to_translation()
        rot = obj.rotation_euler  # same as GUI values (in radians)
        scale = obj.scale

        # Compute radius using world-space vertices
        verts_world = [obj.matrix_world @ v.co for v in obj.data.vertices]
        origin = obj.matrix_world.to_translation()
        radius = max((v - origin).length for v in verts_world)

        return {
            "shape": "SPHERE",
            "location": vector_to_list(loc),
            "rotation_euler_rad": [float(rot.x), float(rot.y), float(rot.z)],  # same as GUI
            "scale": vector_to_list(scale),
        }
    elif mesh_type == "CUBE":
        loc = obj.matrix_world.to_translation()
        rot = obj.rotation_euler  # same rotation as shown in GUI (in radians)
        scale = obj.scale         # 3D scale vector

        return {
            "shape": "CUBE",
            "translation": vector_to_list(loc),
            "rotation_euler_rad": [float(rot.x), float(rot.y), float(rot.z)],
            "scale": vector_to_list(scale),  # now exports full 3D scale
        }
    elif mesh_type == "PLANE":
        verts_world = [obj.matrix_world @ v.co for v in obj.data.vertices]
        corners = [vector_to_list(v) for v in verts_world]
        return {
            "shape": "PLANE",
            "corners_world": corners,
        }
    else:
        return None

def get_blend_object_as_dict():
    """Extract scene info into a JSON-serializable dictionary."""
    scene = bpy.context.scene
    blend_object = {
        "scene_name": scene.name,
        "objects": []
    }

    for obj in bpy.data.objects:
        if obj.hide_get():  # skip hidden objects
            continue

        entry = {"name": obj.name, "type": obj.type}

        if obj.type == "CAMERA":
            cam = obj.data
            entry.update({
                "location": vector_to_list(obj.matrix_world.to_translation()),
                "gaze_vector_direction": vector_to_list(get_camera_gaze_vector(obj)),
                "up_vector": vector_to_list(obj.matrix_world.to_quaternion() @ mathutils.Vector((0.0, 1.0, 0.0))),
                "focal_length": float(cam.lens),
                "sensor_width": float(cam.sensor_width),
                "sensor_height": float(cam.sensor_height),
                "sensor_fit":cam.sensor_fit,
                "film_resolution": [
                    int(scene.render.resolution_x),
                    int(scene.render.resolution_y)
                ],

            })

        elif obj.type == "LIGHT" and obj.data.type == "POINT":
            entry.update({
                "location": vector_to_list(obj.matrix_world.to_translation()),
                "radiant_intensity": float(obj.data.energy)
            })

        elif obj.type == "MESH":
            mesh_data = get_mesh_data(obj)
            if mesh_data:
                entry.update(mesh_data)
            else:
                # skip unrecognized mesh types
                continue

        else:
            continue  # skip other types

        blend_object["objects"].append(entry)

    return blend_object

if __name__ == "__main__":
    blend_object = get_blend_object_as_dict()

    with open(OUTPUT_FILE_PATH, "w", encoding="utf-8") as file:
        json.dump(blend_object, file, indent=4)

    print(f"Scene exported to {OUTPUT_FILE_PATH}")
