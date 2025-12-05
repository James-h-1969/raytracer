# Raytracer Source Code 
The code in this folder is directly responsible for turning the output json file from blender 
into an image. 

## Running the code (Build instructions)
In order to run everything, including converting the file into a PNG, you can run
```bash
./build_run_convert <filename>
```
Where filename is everything _before_ the .json extension (eg. test1.json -> test1). In 
order to adjust the settings for this, change line 18 of build_run_convert.sh.

This will output the generated image in the Output folder.

## Running unit tests
While in the `build` folder, do the following 
```bash
cmake ..
make
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