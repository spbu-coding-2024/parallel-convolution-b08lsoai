# Convolution

Image convolution tool with multiple parallelization strategies.

## Features

- **Filters**: `blur`, `sharpen`, `gaussian`, `motion`, `edge`, `emboss`
- **Modes**: 
  - `seq` - sequential
  - `row` - parallel by rows
  - `column` - parallel by columns  
  - `pixel` - parallel by individual pixels
  - `block` - parallel by blocks (64x64)
- **Pipeline mode**: `--queue` / `-q` - process multiple images with reader/worker/writer threads
- **Formats**: `jpeg`, `jpg`, `png`, `bmp`, `tga`

## Build

```bash
make build
````

## Usage

```bash
./build/convol <input_file> --filter=<filter> --mode=<mode> [--help | -h] [--queue | -q]
````

## Examples

```bash
# Sequential blur on an image
./build/convol image.jpg --filter=blur --mode=seq

# Parallel by blocks with motion blur
./build/convol image.png --filter=motion --mode=block

# Emboss effect with row-parallel execution
./build/convol photo.jpg --filter=emboss --mode=row

# Pipeline mode: process all images in ./images folder
./build/convol ./images/*.jpg --filter=gaussian --mode=pixel --queue

# Parallel blocks with motion blur for multiple files
./build/convol image.png photo.png file.bmp --filter=motion --mode=block
```

## Tests

```
make test
```
## Benchmarks

**Single image benchmark (comparison of modes)**
```bash
# Default (filter=motion, image=./images/test_image.jpg)
make bench
# Custom filter and image
make bench FILTER=blur IMAGE=./images/photo.png
make bench FILTER=gaussian IMAGE=./images/image.jpg
```
**Pipeline mode benchmark (Standard vs Queue)**
```bash
# Default (filter=motion, image=./images/test_image.jpg, mode=block)
make bench-queue
# Custom filter and image
make bench-queue FILTER_Q=blur MODE_Q=block IMAGE="./images/*.jpg"
```

Benchmark results are located in a [corresponding](./benchmark_res/) folder