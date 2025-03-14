# Map Split Tool

## Run
options

| Option       | Description              |
|--------------|--------------------------|
| --input_path | Input sfm map directory  |
| --output_path | Output sfm map directory |
| --saved_names | image names to be saved  |


example usage: 

Save the map including images in `VID_20200821_125942` `VID_20200821_13004` to `sfm_workspace_split`:
```commandline
python split_map_by_name.py \
    -input_path ./sfm_workspace
    -output_path  ./sfm_workspace_split \
    -saved_names VID_20200821_125942 VID_20200821_130046
    
   
````
