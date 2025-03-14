# Face Mask Tools

## Requirements
```commandline
pip install opencv-python torch ultralytics tqdm
```

## Auto Run
options

| Option           | Description                                    |
|------------------|------------------------------------------------|
| --input_dir, -i  | Input image directory                          |
| --output_dir, -o | Output image directory                         |
| --device, -d     | Device to run on (cpu/cuda), default: "cuda:0" |

 
input data structure:
```
images
├── image1.jpg
├── image2.jpg
├── folder1
│   ├── image3.jpg
│   ├── image4.jpg
│   └── folder2
│       ├── image5.jpg
...

```

example usage:
```commandline
python mask.py \
    -i ./images
    -o ./masked
    -d cuda:0
   
````

output data structure:
```
masked
├── image1.jpg
├── image2.jpg
├── folder1
│   ├── image3.jpg
│   ├── image4.jpg
│   └── folder2
│       ├── image5.jpg
...
```

## Manual mask
The manual masking tool is interactive and can directly change the original image.  
```commandline
python manual_mask.py \
    -i ./images
````
