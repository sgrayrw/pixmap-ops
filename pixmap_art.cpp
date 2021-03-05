#include <iostream>
#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
    ppm_image image;
    image.load("../images/cat-ascii.ppm");

    ppm_image rotated = image.rotate();
    rotated.save("rotated.ppm");

    ppm_image inverted = image.invert();
    inverted.save("inverted.ppm");

    ppm_image border = image.border({255, 0, 0}, 25);
    border.save("border.ppm");
}
