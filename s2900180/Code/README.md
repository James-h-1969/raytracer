# Raytracer Source Code 
The code in this folder is directly responsible for turning the output json file from blender 
into an image. 

## Running the code (Build instructions)
```bash
./build_run_convert <filename>
```

## Running unit tests
While in the `build` folder and _after_ building the project, do the following 
```bash
./test/unit/run_tests
```

## Running speed tests
Similarly while in the `build` folder and _after_ building the project, do the following 
```bash
./test/speed/test_speed
```


## Converting images 
When the final code outputs a `.ppm` file, you can convert it into a png using 
```bash
convert output.ppm output.png
```

## TODO:
    - Handle refraction 
    - Texture and texture mapping 
        - Make it so values of ka, ks, kd are better
        - Check the normals of cubes
        - Rotations of cubes
    - Further tooling from command line 
        - depth of recursion (default 3)
        - amount of samples for anti aliasing (default 1)
    - Distributed ray tracing 
    - Lens effects 