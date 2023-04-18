import os
import subprocess

from matplotlib import pyplot as plt


def svg2emf(filename):
    # Save the SVG file
    plt.savefig(filename, format="svg", transparent="ture", dpi=300, bbox_inches='tight')


    # Convert the SVG file to EMF format using Inkscape
    subprocess.call(["inkscape", "--export-type=emf", filename])

    # Delete the SVG file
    # os.remove(filename)

def svg2bmp(filename):
        # Convert the SVG file to EMF format using Inkscape
        subprocess.call(["inkscape", "--export-type=bmp", filename])

        # Delete the SVG file
        # os.remove(filename)