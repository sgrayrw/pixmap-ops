#include "ppm_image.h"
#include <fstream>
#include <iostream>
#include <list>

using namespace agl;
using namespace std;

ppm_image::ppm_image() {
    pixels = nullptr;
    _width = _height = -1;
    max_color = -1;
}

ppm_image::ppm_image(int w, int h) {
    assert(w >= 0 && h >= 0);

    pixels = new ppm_pixel *[h];
    for (int i = 0; i < h; ++i) {
        pixels[i] = new ppm_pixel[w];
    }
    _width = w;
    _height = h;
    max_color = -1;
}

ppm_image::ppm_image(const ppm_image &orig) {
    int w = orig._width;
    int h = orig._height;
    pixels = new ppm_pixel *[h];
    for (int i = 0; i < h; ++i) {
        pixels[i] = new ppm_pixel[w];
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = orig.pixels[i][j];
        }
    }
    _width = w;
    _height = h;
    version = orig.version;
    max_color = orig.max_color;
}

ppm_image &ppm_image::operator=(const ppm_image &orig) {
    // protect against self-assignment
    if (&orig == this) {
        return *this;
    }

    cleanup();
    int w = orig._width;
    int h = orig._height;
    pixels = new ppm_pixel *[h];
    for (int i = 0; i < h; ++i) {
        pixels[i] = new ppm_pixel[w];
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = orig.pixels[i][j];
        }
    }
    _width = w;
    _height = h;
    version = orig.version;
    max_color = orig.max_color;

    return *this;
}

ppm_image::~ppm_image() {
    cleanup();
}

void ppm_image::cleanup() {
    if (_width != -1 && _height != -1) {
        for (int i = 0; i < _height; ++i) {
            delete[] pixels[i];
        }
        delete[] pixels;
    }
}

bool ppm_image::load(const std::string &filename) {
    try {
        cleanup();

        ifstream file(filename);
        file >> version;
        file >> _width;
        file >> _height;
        file >> max_color;

        pixels = new ppm_pixel *[_height];
        for (int i = 0; i < _height; ++i) {
            pixels[i] = new ppm_pixel[_width];
            for (int j = 0; j < _width; ++j) {
                int r, g, b;
                file >> r >> g >> b;
                pixels[i][j].r = '\0' + r;
                pixels[i][j].g = '\0' + g;
                pixels[i][j].b = '\0' + b;
            }
        }

        file.close();
        return true;
    } catch (const ifstream::failure& e) {
        cout << "Exception opening/reading file" << e.what() << endl;
        return false;
    }
}

bool ppm_image::save(const std::string &filename) const {
    try {
        ofstream file(filename);
        file << version << endl;
        file << _width << " " << _height << endl;
        file << max_color << endl;
        for (int i = 0; i < _height; ++i) {
            for (int j = 0; j < _width; ++j) {
                file << +pixels[i][j].r << " "
                     << +pixels[i][j].g << " "
                     << +pixels[i][j].b << " "
                     << endl;
            }
        }

        file.close();
        return true;
    } catch (const ofstream::failure& e) {
        cout << "Exception opening/reading file" << e.what() << endl;
        return false;
    }
}

ppm_image ppm_image::resize(int w, int h) const {
    assert(w >= 0 && h >= 0);
    ppm_image result(w, h);
    result.version = version;
    result.max_color = max_color;

    if (_height == 0 || _width == 0) {
        return result;
    }

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int _i = 0, _j = 0; // loc in original img
            if (h != 1) {
                _i = floor((float) i / (h - 1) * (_height - 1));
            }
            if (w != 1) {
                _j = floor((float) j / (w - 1) * (_width - 1));
            }
            result.pixels[i][j] = pixels[_i][_j];
        }
    }
    return result;
}

ppm_image ppm_image::flip_horizontal() const {
    ppm_image result(*this);
    for (int j = 0; j < result._width; ++j) {
        for (int i = 0; i < result._height / 2; ++i) {
            ppm_pixel tmp = result.pixels[i][j];
            result.pixels[i][j] = result.pixels[result._height - i - 1][j];
            result.pixels[result._height - i - 1][j] = tmp;
        }
    }
    return result;
}

ppm_image ppm_image::subimage(int startx, int starty, int w, int h) const {
    assert(startx >= 0 && startx < _height && starty >= 0 && starty <= _width);
    assert(w >= 0 && h >= 0);

    h = min(h, _height - startx);
    w = min(w, _width - starty);

    ppm_image result(w, h);
    result.version = version;
    result.max_color = max_color;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            result.pixels[i][j] = pixels[startx + i][starty + j];
        }
    }

    return result;
}

void ppm_image::replace(const ppm_image &image, int startx, int starty) {
    assert(startx >= 0 && startx < _height && starty >= 0 && starty < _width);

    int h = min(_height, startx + image._height);
    int w = min(_width, starty + image._width);

    for (int i = startx; i < h; ++i) {
        for (int j = starty; j < w; ++j) {
            pixels[i][j] = image.pixels[i - startx][j - starty];
        }
    }
}

ppm_image ppm_image::alpha_blend(const ppm_image &other, float alpha) const {
    auto apply_alpha = [&](unsigned char this_v, unsigned char other_v) {
        return '\0' + (int) (this_v * (1 - alpha) + other_v * alpha);
    };

    assert(_width == other._width && _height == other._height);
    ppm_image result(*this);

    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            const ppm_pixel& this_p = pixels[i][j];
            const ppm_pixel& other_p = other.pixels[i][j];
            ppm_pixel& result_p = result.pixels[i][j];
            result_p.r = apply_alpha(this_p.r, other_p.r);
            result_p.g = apply_alpha(this_p.g, other_p.g);
            result_p.b = apply_alpha(this_p.b, other_p.b);
        }
    }

    return result;
}

ppm_image ppm_image::gammaCorrect(float gamma) const {
    auto apply_gamma = [&](unsigned char val) {
        return '\0' + (int) (255 * pow((float) val / 255.0f, (1 / gamma)));
    };

    ppm_image result(*this);
    for (int i = 0; i < result._height; ++i) {
        for (int j = 0; j < result._width; ++j) {
            ppm_pixel& p = result.pixels[i][j];
            p.r = apply_gamma(p.r);
            p.g = apply_gamma(p.g);
            p.b = apply_gamma(p.b);
        }
    }
    return result;
}

ppm_image ppm_image::grayscale() const {
    ppm_image result(*this);
    for (int i = 0; i < result._height; ++i) {
        for (int j = 0; j < result._width; ++j) {
            ppm_pixel& p = result.pixels[i][j];
            int c = floor(0.3 * (int) p.r + 0.59 * (int) p.g + 0.11 * (int) p.b);
            p.r = p.g = p.b = '\0' + c;
        }
    }
    return result;
}

ppm_pixel ppm_image::get(int row, int col) const {
    assert(row >= 0 && row < _height && col >= 0 && col < _width);
    return pixels[row][col];
}

void ppm_image::set(int row, int col, const ppm_pixel &c) {
    assert(row >= 0 && row < _height && col >= 0 && col < _width);
    pixels[row][col] = c;
}

int ppm_image::height() const {
    return _height;
}

int ppm_image::width() const {
    return _width;
}

ppm_image ppm_image::rotate() {
    ppm_image result(_height, _width);
    result.version = version;
    result.max_color = max_color;
    // algorithm adapted from https://www.careercup.com/question?id=5667482614366208
    for (int i = 0; i < _width; ++i) {
        for (int j = 0; j < _height; ++j) {
            result.pixels[j][_width - i - 1] = pixels[i][j];
        }
    }
    return result;
}

ppm_image ppm_image::invert() {
    ppm_image result(*this);
    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            ppm_pixel& p = result.pixels[i][j];
            p.r = (unsigned char) (max_color - (int) p.r);
            p.g = (unsigned char) (max_color - (int) p.g);
            p.b = (unsigned char) (max_color - (int) p.b);
        }
    }
    return result;
}

ppm_image ppm_image::border(const ppm_pixel &color, int width) {
    assert(width >= 0);

    ppm_image result(_width + width * 2, _height + width * 2);
    result.version = version;
    result.max_color = max_color;

    for (int i = 0; i < result._height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.pixels[i][j] = color;
            result.pixels[i][result._width - 1 - j] = color;
        }
    }

    for (int j = 0; j < result._width; ++j) {
        for (int i = 0; i < width; ++i) {
            result.pixels[i][j] = color;
            result.pixels[result._height - 1 - i][j] = color;
        }
    }

    result.replace(*this, width, width);
    return result;
}

ppm_image ppm_image::box_blur(int n) {
    auto get_average = [&](const ppm_image& img, int i, int j) {
        int sum_r = 0, sum_g = 0, sum_b = 0, count = 0;
        int range[n];
        for (int k = 0; k < n; ++k) {
            range[k] = k - n / 2;
        }

        for (auto dx : range) {
            for (auto dy : range) {
                int x = i + dx, y = j + dy;
                if (x >= 0 && x < _height && y >= 0 && y < _width) {
                    sum_r += (int) img.pixels[x][y].r;
                    sum_g += (int) img.pixels[x][y].g;
                    sum_b += (int) img.pixels[x][y].b;
                    count++;
                }
            }
        }
        return ppm_pixel{
            (unsigned char) (sum_r / count),
            (unsigned char) (sum_g / count),
            (unsigned char) (sum_b / count)
        };
    };

    ppm_image result(*this);
    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            result.pixels[i][j] = get_average(result, i, j);
        }
    }
    return result;
}

ppm_image ppm_image::swirl_color() {
    ppm_image result(*this);
    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            ppm_pixel& p = result.pixels[i][j];
            auto tmp = p.b;
            p.b = p.g;
            p.g = p.r;
            p.r = tmp;
        }
    }
    return result;
}

ppm_image ppm_image::overlay(ppm_pixel color) {
    ppm_image overlay(*this);
    for (int i = 0; i < _height; ++i) {
        for (int j = 0; j < _width; ++j) {
            overlay.pixels[i][j] = color;
        }
    }
    return alpha_blend(overlay, 0.5);
}
