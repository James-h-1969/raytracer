# Export.py 
# This script takes a .blend scene and processes all the 
# import objects, creating a .json object with all lights,
# camera and object details.
#
# James Hocking 2025, assisted w. ChatGPT

import bpy 
import json 

### PARAMETERS: Only change here 
OUTPUT_FILE_PATH="output.json"
################################

def get_blend_object_as_dict() -> dict:
    """
    This function uses the bpy library to process the objects 
    in the scene and add them to a dictionary that can be 
    easily converted into a .json file.
    """

    blend_object = {
        "objects":[]
    }

    for object in bpy.data.objects:
        if not object.hide_get(): # only export visible objects
            object_type: str = object.type
            current_object = {
                "type":object_type
            }

            if object_type == "camera":
                current_object["location"] = ...
                current_object["gaze_vector_direction"] = ...
                current_object["focal_length"] = ...
                current_object["sensor_width"] = ...
                current_object["sensor_height"] = ...
                current_object["film_resolution"] = ...
            if object_type == "light":
                ...

            blend_object["objects"].append(current_object)
    
    return blend_object


if __name__ == "__main__":
    blend_object = get_blend_object_as_dict()

    with open(OUTPUT_FILE_PATH, "w") as file:
        json.dump(blend_object, file, indent=4)
