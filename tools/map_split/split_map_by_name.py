import os
import sys
from pathlib import Path

sys.path.append(str(Path(os.getcwd()).parent))

import numpy as np
from read_write_model import *


def get_part_image_ids(images, saved_names):
    ret_image_id = []
    name_to_id = {images[image_id].name: image_id for image_id in images}
    for name in name_to_id:
        for saved_name in saved_names:
            if name.find(saved_name) != -1:
                ret_image_id.append(name_to_id[name])
                continue

        # time = int(name.split('/')[-1].split('_')[0])
        # if time > beg and time < end:
        #     ret_image_id.append(name_to_id[name])
    return ret_image_id


def extract_part_image(images, image_ids, points):
    new_images = {}
    image_id_set = set(image_ids)
    for image_id in images:
        if image_id not in image_id_set:
            continue
        image = images[image_id]
        new_point3D_ids = []
        for pid in image.point3D_ids:
            if pid not in points:
                new_point3D_ids.append(-1)
            else:
                new_point3D_ids.append(pid)
        new_point3D_ids = np.array(new_point3D_ids)
        new_images[image_id] = Image(
            id=image_id, qvec=image.qvec, tvec=image.tvec,
            camera_id=image.camera_id, name=image.name,
            xys=image.xys, point3D_ids=new_point3D_ids
        )
    return new_images


def extract_part_points(points, image_ids):
    new_points = {}
    image_id_set = set(image_ids)
    for pid in points:
        new_image_ids, new_point2D_idxs = [], []
        point = points[pid]
        for i, image_id in enumerate(point.image_ids):
            if image_id in image_id_set:
                new_image_ids.append(image_id)
                new_point2D_idxs.append(point.point2D_idxs[i])
        if len(new_image_ids) < 2:
            continue
        new_image_ids = np.array(new_image_ids)
        new_point2D_idxs = np.array(new_point2D_idxs)
        new_points[pid] = Point3D(id=pid, xyz=point.xyz,
                                  rgb=point.rgb, error=point.error,
                                  image_ids=new_image_ids, point2D_idxs=new_point2D_idxs)
    return new_points


def main(input_path, output_path, saved_names=[]):
    images_bin = read_images_binary(os.path.join(input_path, 'images.bin'))
    points_bin = read_points3d_binary(os.path.join(input_path, 'points3D.bin'))

    save_image_ids = get_part_image_ids(images_bin, saved_names)
    print('save_image_ids:', len(save_image_ids))

    save_points_bin = extract_part_points(points_bin, save_image_ids)
    save_images_bin = extract_part_image(images_bin, save_image_ids, save_points_bin)

    write_images_binary(save_images_bin, os.path.join(output_path, 'images.bin'))
    write_points3d_binary(save_points_bin, os.path.join(output_path, 'points3D.bin'))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input_path', type=str, required=True)
    parser.add_argument('--output_path', type=str, required=True)
    parser.add_argument('--saved_names', type=str, required=True, nargs='+')
    args = parser.parse_args()
    main(args.input_path, args.output_path, args.saved_names)