import argparse
import cv2
import os
import numpy as np
import glob

def apply_mosaic(roi, block_size=50):
    """Apply mosaic effect to selected region"""
    h, w = roi.shape[:2]
    small_roi = cv2.resize(roi, (w//block_size, h//block_size), interpolation=cv2.INTER_NEAREST)
    return cv2.resize(small_roi, (w, h), interpolation=cv2.INTER_NEAREST)

# Global mouse state tracking
drawing = False
start_x, start_y = -1, -1
end_x, end_y = -1, -1

def mouse_callback(event, x, y, flags, param):
    """Handle scaled mouse events for ROI selection"""
    global start_x, start_y, end_x, end_y, drawing
    scale, orig_w, orig_h = param
    
    # Convert coordinates and clamp to image boundaries
    x_orig = int(x / scale)
    y_orig = int(y / scale)
    x_orig = max(0, min(x_orig, orig_w - 1))
    y_orig = max(0, min(y_orig, orig_h - 1))
    
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        start_x, start_y = x_orig, y_orig
        end_x, end_y = x_orig, y_orig
    elif event == cv2.EVENT_MOUSEMOVE and drawing:
        end_x, end_y = x_orig, y_orig
    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False
        end_x, end_y = x_orig, y_orig

def find_image_files(input_dir):
    """Recursively find all image files in directory"""
    image_extensions = ('*.jpg', '*.jpeg', '*.png', '*.bmp', '*.tif', '*.tiff')
    image_paths = []
    base_path = os.path.normpath(input_dir)
    
    for ext in image_extensions:
        search_pattern = os.path.join(base_path, "**", ext)
        for abs_path in glob.iglob(search_pattern, recursive=True):
            if os.path.isfile(abs_path):
                rel_path = os.path.relpath(abs_path, base_path)
                image_paths.append((abs_path, rel_path))
    
    print(f"Found {len(image_paths)} images in directory tree")
    return image_paths

def process_image_directory(input_dir):
    """Main processing workflow with scaled display"""
    global start_x, start_y, end_x, end_y
    
    image_files = find_image_files(input_dir)
    
    for abs_path, rel_path in image_files:
        original = cv2.imread(abs_path)
        if original is None:
            print(f"Error reading image: {abs_path}")
            continue
        
        image = original.copy()
        h, w = image.shape[:2]
        
        # Calculate display scaling
        max_display_width = 1200
        max_display_height = 950
        scale = min(max_display_width / w, max_display_height / h)
        display_w = int(w * scale)
        display_h = int(h * scale)
        
        window_title = f'Editing: {rel_path} (ESC to skip)'
        cv2.namedWindow(window_title, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(window_title, display_w, display_h)
        
        # Set mouse callback with scaling parameters
        cv2.setMouseCallback(window_title, mouse_callback, param=(scale, w, h))
        
        while True:
            # Create scaled display image
            display_image = cv2.resize(image.copy(), (display_w, display_h))
            
            # Draw selection rectangle
            if start_x != -1 and end_x != -1:
                x1 = int(start_x * scale)
                y1 = int(start_y * scale)
                x2 = int(end_x * scale)
                y2 = int(end_y * scale)
                cv2.rectangle(display_image, (x1, y1), (x2, y2), (0, 255, 0), 2)
            
            cv2.imshow(window_title, display_image)
            
            key = cv2.waitKey(1) & 0xFF
            if key == 27:  # ESC to skip
                break
            elif key in (13, 32):  # Enter/Space to confirm
                if start_x != -1 and end_x != -1:
                    x1, x2 = sorted([start_x, end_x])
                    y1, y2 = sorted([start_y, end_y])
                    
                    # Validate selection bounds
                    if x2 > x1 and y2 > y1 and x1 >= 0 and y1 >= 0 and x2 <= w and y2 <= h:
                        selected_roi = original[y1:y2, x1:x2]
                        mosaic_roi = apply_mosaic(selected_roi)
                        image[y1:y2, x1:x2] = mosaic_roi
                        cv2.imwrite(abs_path, image)
                        print(f"Saved: {abs_path}")
                        break
                    else:
                        print("Invalid region selected")
                else:
                    print("No region selected")
        
        cv2.destroyAllWindows()
        # Reset coordinates for next image
        start_x = start_y = end_x = end_y = -1

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Interactive image mosaic tool with scaled display')
    parser.add_argument("-i", "--input_dir", 
                       type=str, 
                       required=True,
                       help="Root directory containing images (processes subfolders recursively)")
    args = parser.parse_args()
    
    process_image_directory(args.input_dir)
    print("Processing complete!")
