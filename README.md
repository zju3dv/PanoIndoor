# PanoIndoor

> Indoor large-scale panoramic visual localization dataset   
> Hailin Yu, Jiarun Liu, Zhichao Ye, Xinyu Chen, Ruohao Zhan, Yichun Shentu, Zhongyun Lu, Guofeng Zhang  
> JIG 2025


# Dataset Link

```
通过网盘分享的文件：PanoIndoor
链接: https://pan.baidu.com/s/1JUtCXu6yTr0rJaFPCFrGkA?pwd=kp54 提取码: kp54 
--来自百度网盘超级会员v7的分享
```

# Dataset Structure
Our dataset contains four scenes:
- office
- canteen
- corridor
- parkinglot

Each scene contains files in following structure:
```
.
├── images                      # all panorama images
│   └── insta_pano_default
├── query_with_intrinsics.txt   # query images with pose
└── sfm_workspace               # reconstructed map with all images
    ├── cameras.bin
    ├── images.bin
    └── points3D.bin
```

# Tools
We also provide three useful tools for data process:
* `tools\face_mask` is for data desensitization. We have mosaic processed the visible faces in the dataset. If there are more advanced privacy protection requirements, manual processing can be performed.
* `tools\map_split` is for spliting the maps in the `sfm_workspace` based on the selected reference image folder name. During localizaiton, we only need the reconstruction results of the references, but not the queries.
* `tools\panorama` is for changing panorama images to perspective images.  
Please check `README.md` in each folder for detailed usage.
 

# Citation
If you find this code useful for your research, please use the following BibTeX entry.

```
```

