# Convolution

Image convolution tool with multiple parallelization strategies.

## Features

- **Filters**: `blur`, `sharpen`, `gaussian`, `motion`, `edge`, `emboss`
- **Modes**: 
  - `seq` - sequential
- **Formats**: `jpeg`, `jpg`, `png`, `bmp`, `tga`

## Build

```bash
make build
````

## Usage

```bash
./build/convol <input_file> --filter=<filter> --mode=<mode> [--help | -h]
````

## Examples

```bash
# Sequential blur on an image
./build/convol image.jpg --filter=blur --mode=seq
``
