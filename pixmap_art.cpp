#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
    ppm_image image;
    image.load("../images/cat-ascii.ppm");
    //image.rotate().save("rotate.ppm");
    //image.invert().save("invert.ppm");
    //image.border({255, 0, 0}, 25).save("border.ppm");
    //image.box_blur(25).save("box_blur.ppm");
    //image.swirl_color().save("swirl.ppm");
    //image.overlay({191, 139, 225}).save("overlay.ppm");

    int x = 40, y = 40, w = 400, h = 850;
    ppm_image background = image.subimage(x, y, w, h);
    ppm_image tom_and_jerry;
    tom_and_jerry.load("../images/tom-and-jerry-ascii.ppm");
    tom_and_jerry = tom_and_jerry.resize(w, h);
    ppm_image blend = background.alpha_blend(tom_and_jerry, 0.8f);
    image.replace(blend, x, y);
    image.save("artwork.ppm");
}
