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
OUTPUT_FILE_PATH = "/home/jhocking542/uni/computer-graphics/raytracer/s2900180/ASCII/test2.json"
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

def extract_texture_path_from_material(mat):
    if not mat or not mat.use_nodes:
        return None

    for node in mat.node_tree.nodes:
        if node.type == 'TEX_IMAGE' and node.image:
            path = bpy.path.abspath(node.image.filepath)
            path = path[:-4]
            path += ".ppm"
            return {
                "absolute_path": path,
            }
     
    return None

def extract_phong_from_material(mat):
    """
    Extracts Phong parameters and handles Texture * Color multiplication logic.
    Also handles MIX_SHADER by averaging the base colours.
    """
     
    if not mat or not mat.use_nodes:
        return None

    # --- Defaults ---
    output_data = {
        "ka": 0.3,
        "kd": 0.5,
        "ks": 0.5,
        "shininess": 0.0,
        "reflectivity": 0.3,
        "transparancy": 0.0,
        "ior": 1.0,
        "base_colour": [0, 0, 0], # Default black
        # Texture is optional, added only if found
    }

    # --- Helper: Analyze a specific color socket ---
    # Returns a tuple: (found_color_vector, found_texture_path)
    def analyze_color_socket(socket, default_val):
        # 1. If no link, just return the color picker value
        if not socket.is_linked:
            return (default_val, None)
         
        # 2. Trace the link
        node = socket.links[0].from_node
         
        # CASE A: Direct Texture Link
        # If linked directly to an image, the "Tint" is White (1.0)
        if node.type == 'TEX_IMAGE' and node.image:
            path = bpy.path.abspath(node.image.filepath)
            path = path[:-4] + ".ppm" # Enforce .ppm extension
            return ([1.0, 1.0, 1.0, 1.0], {"absolute_path": path})

        # CASE B: MixRGB (Multiply) -> Texture * Tint
        elif node.type == 'MIX_RGB' and node.blend_type == 'MULTIPLY':
            # We need to find which input is color and which is texture
            col_input = node.inputs[1] # "Color1"
            tex_input = node.inputs[2] # "Color2"
             
            found_color = [1.0, 1.0, 1.0, 1.0]
            found_path = None

            # Check Input 1
            if not col_input.is_linked:
                found_color = col_input.default_value
            elif col_input.links[0].from_node.type == 'TEX_IMAGE':
                img_node = col_input.links[0].from_node
                if img_node.image:
                    p = bpy.path.abspath(img_node.image.filepath)
                    found_path = {"absolute_path": p[:-4] + ".ppm"}

            # Check Input 2
            if not tex_input.is_linked:
                # If we didn't find color in input 1, this might be it
                c = tex_input.default_value
                found_color = [found_color[0]*c[0], found_color[1]*c[1], found_color[2]*c[2], 1.0]
            elif tex_input.links[0].from_node.type == 'TEX_IMAGE':
                img_node = tex_input.links[0].from_node
                if img_node.image:
                    p = bpy.path.abspath(img_node.image.filepath)
                    found_path = {"absolute_path": p[:-4] + ".ppm"}

            return (found_color, found_path)

        # Fallback: Unknown node type linked
        return (default_val, None)

    # --- Helper: Extract color from a BSDF node directly ---
    def get_color_from_bsdf(bsdf_node):
        if not bsdf_node: return ([0.0, 0.0, 0.0, 1.0], None)
        col_sock = None
        if "Base Color" in bsdf_node.inputs:
            col_sock = bsdf_node.inputs["Base Color"]
        elif "Color" in bsdf_node.inputs:
            col_sock = bsdf_node.inputs["Color"]
        
        if col_sock:
            return analyze_color_socket(col_sock, col_sock.default_value)
        return ([0.0, 0.0, 0.0, 1.0], None)

    # --- 1. Identify Nodes (Mix Shader vs Standard) ---
    target_node = None
    mix_node = None
    
    # First, look for a Mix Shader
    for node in mat.node_tree.nodes:
        if node.type == 'MIX_SHADER':
            mix_node = node
            break
    
    override_color = None
    override_texture = None

    if mix_node:
        # Handle Mix Shader Logic
        fac = mix_node.inputs['Fac'].default_value
        
        # Get the two shaders
        shader1 = mix_node.inputs[1].links[0].from_node if mix_node.inputs[1].is_linked else None
        shader2 = mix_node.inputs[2].links[0].from_node if mix_node.inputs[2].is_linked else None
        
        # Get colors from both
        c1, tex1 = get_color_from_bsdf(shader1)
        c2, tex2 = get_color_from_bsdf(shader2)
        
        # Calculate Weighted Average: (C1 * (1-Fac)) + (C2 * Fac)
        inv_fac = 1.0 - fac
        mixed_r = (c1[0] * inv_fac) + (c2[0] * fac)
        mixed_g = (c1[1] * inv_fac) + (c2[1] * fac)
        mixed_b = (c1[2] * inv_fac) + (c2[2] * fac)
        
        override_color = [mixed_r, mixed_g, mixed_b]
        
        # Use texture from the dominant shader (simplification)
        override_texture = tex2 if fac > 0.5 else tex1
        
        # Set target_node to the dominant shader so we can still extract Roughness/IOR
        target_node = shader2 if fac > 0.5 else shader1

    else:
        # No Mix Shader, find standard BSDF
        for node in mat.node_tree.nodes:
            if node.type in ['BSDF_PRINCIPLED', 'BSDF_DIFFUSE', 'BSDF_GLOSSY']:
                target_node = node
                break
            
    # --- 2. Extract Data ---
    if target_node:
        # --- Handle Color ---
        if override_color:
            # We calculated a mix
            output_data["base_colour"] = [
                int(override_color[0] * 255), 
                int(override_color[1] * 255), 
                int(override_color[2] * 255)
            ]
            if override_texture:
                output_data["texture"] = override_texture
        else:
            # Standard single node extraction
            color_socket = None
            if "Base Color" in target_node.inputs:
                color_socket = target_node.inputs["Base Color"]
            elif "Color" in target_node.inputs:
                color_socket = target_node.inputs["Color"]
                
            if color_socket:
                col_val, tex_data = analyze_color_socket(color_socket, color_socket.default_value)
                output_data["base_colour"] = [
                    int(col_val[0] * 255), 
                    int(col_val[1] * 255), 
                    int(col_val[2] * 255)
                ]
                if tex_data:
                    output_data["texture"] = tex_data

        # --- Extract other Physics (Roughness/IOR) from the target node ---
        if "Roughness" in target_node.inputs:
            roughness = target_node.inputs["Roughness"].default_value
            output_data["shininess"] = (1.0 - roughness ** 4) * 2048
            
        if "IOR" in target_node.inputs:
            output_data["ior"] = target_node.inputs["IOR"].default_value
            
        if "Transmission" in target_node.inputs:
             output_data["transparancy"] = target_node.inputs["Transmission"].default_value

    return output_data

def get_mesh_data(obj):
    """Extract geometric info depending on mesh type."""
    mesh_type = get_mesh_type(obj)
    if mesh_type == "SPHERE":
        # Get transform components
        loc = obj.matrix_world.to_translation()
        rot = obj.rotation_euler  # same as GUI values (in radians)
        scale = obj.scale
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
            light = obj.data
            intensity_factor = float(light.energy) / (4.0 * math.pi)
            r, g, b = light.color
            i_d = r * intensity_factor
            i_s = r * intensity_factor
            entry.update({
                "location": vector_to_list(obj.matrix_world.to_translation()),
                "id": i_d,
                "is": i_s
            })
        elif obj.type == "MESH":
            mesh_data = get_mesh_data(obj)
            if mesh_data:
                entry.update(mesh_data)
                material_data = extract_phong_from_material(obj.active_material)
                if material_data:
                    entry["material"] = material_data
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