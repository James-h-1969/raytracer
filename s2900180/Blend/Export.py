# Export.py 
# This script takes a .blend scene and processes all the 
# import objects, creating a .json object with all lights,
# camera and object details.
#
# James Hocking 2025, assisted w. ChatGPT

import bpy 
import json 

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

if __name__ == "__main__":
    blend_to_json()
