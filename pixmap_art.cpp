#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
    ppm_image image;
    image.load("../images/cat-ascii.ppm");
    image.rotate().save("rotated.ppm");
    image.invert().save("inverted.ppm");
    image.border({255, 0, 0}, 25).save("border.ppm");
    image.box_blur(25).save("box_blurred.ppm");
    image.swirl_color().save("swirled.ppm");
    image.overlay({191, 139, 225}).save("overlay.ppm");
}
