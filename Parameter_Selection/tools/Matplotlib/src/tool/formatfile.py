import os
import subprocess


def svg2emf(filename):
    # Convert the SVG file to EMF format using Inkscape
    subprocess.call(["inkscape", "--export-type=emf", filename])

    # Delete the SVG file
    os.remove(filename)

def svg2bmp(filename):
        # Convert the SVG file to EMF format using Inkscape
        subprocess.call(["inkscape", "--export-type=bmp", filename])

        # Delete the SVG file
        os.remove(filename)