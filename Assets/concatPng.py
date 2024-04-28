import sys
import os
from PIL import Image

def concat_images_horizontally(image_paths, output_name):
    images = [Image.open(image_path) for image_path in image_paths]

    # Determine the total width and maximum height of the output image
    total_width = sum(image.width for image in images)
    max_height = max(image.height for image in images)

    # Create a new image with the required dimensions
    new_image = Image.new('RGBA', (total_width, max_height))

    # Paste each image onto the new image horizontally
    x_offset = 0
    for image in images:
        new_image.paste(image, (x_offset, 0))
        x_offset += image.width

    # Save the resulting image with the specified name
    new_image.save(output_name)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py input_folder output_name")
        sys.exit(1)

    input_folder = sys.argv[1]
    output_name = sys.argv[2]

    # Get all PNG files in the folder and sort them
    image_paths = sorted([os.path.join(input_folder, file) for file in os.listdir(input_folder) if file.endswith('.png')])

    # Concatenate the images horizontally and save the result
    concat_images_horizontally(image_paths, output_name)
