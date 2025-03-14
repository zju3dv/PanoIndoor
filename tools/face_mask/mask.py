import argparse
from copy import deepcopy
import cv2
import os
import glob
import requests
import torch
from tqdm import tqdm
from ultralytics import YOLO


class FaceMasker:
    def __init__(self, input_dir, output_dir,
                 device='cuda:0',
                 threshold=0.1,
                 k=10):
        self.input_dir = input_dir
        self.output_dir = output_dir

        self.device = device if torch.cuda.is_available() else 'cpu'
        self.threshold = threshold
        self.k = k
        # use yolov11n-face.pt
        self.model = self._load_model()

        self.image_files = self.__glob_images()

    def _load_model(self):
        # check model
        if not os.path.exists('yolov11n-face.pt'):
            print("Downloading model...")
            model_url = "https://github.com/akanametov/yolo-face/releases/download/v0.0.0/yolov11n-face.pt"
            filename = os.path.basename(model_url)
            response = requests.get(model_url, stream=True)
            response.raise_for_status()

            with open(filename, "wb") as handle:
                handle.write(response.content)

            print("Model downloaded to yolov11n-face.pt")

        # load model
        print("Loading model from {}".format('yolov11n-face.pt'))

        model_path = os.path.join(os.getcwd(), "yolov11n-face.pt")
        model = YOLO(model_path).to(self.device)
        return model

    def __glob_images(self):
        image_extensions = ('*.jpg', '*.jpeg', '*.png', '*.bmp')
        image_files = []

        for ext in image_extensions:
            pattern = os.path.join(self.input_dir, "**", ext)
            for file in glob.iglob(pattern, recursive=True):
                index = self.input_dir.rstrip('/') + '/'
                image_files.append(file[len(index):])

        print(f"Found {len(image_files)} images in {self.input_dir}")
        return image_files

    @torch.no_grad()
    def __getitem__(self, item):
        image_name = self.image_files[item]
        image = cv2.imread(os.path.join(self.input_dir, image_name))
        image_masked = deepcopy(image)

        h, w, _ = image.shape

        results = self.model(image, imgsz=[w, h])

        for result in results:
            boxes = result.boxes

            for box in boxes:
                bbox = box.xyxy[0].cpu().numpy().astype(int)
                x1, y1, x2, y2 = bbox
                w_ = x2 - x1
                h_ = y2 - y1
                # expand bbox with ratio%
                ratio = 0.25
                x1 = max(0, x1 - int(ratio * w_))
                y1 = max(0, y1 - int(ratio * h_))
                x2 = min(w, x2 + int(ratio * w_))
                y2 = min(h, y2 + int(ratio * h_))

                face = image_masked[y1:y2, x1:x2]
                # mipmap
                face_mosaic = cv2.resize(face, (10, 10), interpolation=cv2.INTER_AREA)
                face_mosaic = cv2.resize(face_mosaic, (x2 - x1, y2 - y1), interpolation=cv2.INTER_NEAREST)
                image_masked[y1:y2, x1:x2] = face_mosaic

        # save masked image
        output_path = os.path.join(self.output_dir, image_name)
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        cv2.imwrite(output_path, image_masked)

        return image_masked


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Mask faces in images')
    parser.add_argument("-i", '--input_dir', type=str, required=True, help='Input directory containing images')
    parser.add_argument('-o', '--output_dir', type=str, required=True, help='Output directory to save masked images')
    parser.add_argument('-d', '--device', type=str, default='cuda:0',
                        help='Device to use for inference (default: cuda:0)')

    args = parser.parse_args()
    face_masker = FaceMasker(input_dir=args.input_dir,
                             output_dir=args.output_dir,
                             device=args.device,
                             )

    for i in tqdm(range(len(face_masker.image_files))):
        pano_image = face_masker[i]














