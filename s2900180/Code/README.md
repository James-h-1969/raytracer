# Raytracer Source Code 
The code in this folder is directly responsible for turning the output json file from blender 
into an image. 

## Running the code
```bash
cmake -B build

cmake --build build

./build/MyCppProject
```

## Converting images 
When the final code outputs a `.ppm` file, you can convert it into a png using 
```bash
convert output.ppm output.png
```

## TODO:
- Export script from blender 
