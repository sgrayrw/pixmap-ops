#include <iostream>
#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
    ppm_image image;
    image.load("../images/cat-ascii.ppm");

    ppm_image rotated = image.rotate();
    rotated.save("rotated.ppm");
}
