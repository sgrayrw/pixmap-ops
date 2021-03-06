#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
    ppm_image image;
    image.load("../images/cat-ascii.ppm");
    image.rotate().save("rotate.ppm");
    image.invert().save("invert.ppm");
    image.border({255, 0, 0}, 25).save("border.ppm");
    image.box_blur(25).save("box_blur.ppm");
    image.swirl_color().save("swirl.ppm");
    image.overlay({191, 139, 225}).save("overlay.ppm");
}
