#ifndef __INTEGRAL_IMAGE_H__
#define __INTEGRAL_IMAGE_H__

#include <vector>
#include "Vector2.h"

template<typename T>
class Image {
private:
    std::vector<std::vector<T>> values;

public:
    inline Image() = default;
    inline Image(int sizeX, int sizeY): values(sizeY, std::vector<T>(sizeX)) {}
    inline T & get(Vector2I p) { return values.at(p.y).at(p.x); }
    inline T get(Vector2I p) const { return values.at(p.y).at(p.x); }
    inline int sizeX() const { return values[0].size(); }
    inline int sizeY() const { return values.size(); }
};

template<typename T> inline std::ostream & operator<<(std::ostream &os, const Image<T> &image) {
    for(int y = 0; y < image.sizeY(); ++y) {
        for(int x = 0; x < image.sizeX(); ++x) {
            os << image.get({x, y}) << " ";
        }
        os << std::endl;
    }
    os << std::endl;
    return os;
}

template<typename T>
class IntegralImage {
private:
    Image<T> image;

public:
    inline IntegralImage() = default;

    explicit inline IntegralImage(Image<T> _image): image(_image.sizeX() + 1, _image.sizeY() + 1) {
        for(int y = 1; y < image.sizeY(); ++y)
            for(int x = 1; x < image.sizeX(); ++x)
                image.get({x, y}) = _image.get({x - 1, y - 1}) + image.get({x - 1, y}) + image.get({x, y - 1}) - image.get({x - 1, y - 1});
    }

    inline T get(Vector2I p0, Vector2I p1) const {
        return image.get({p1.x, p1.y}) - image.get({p0.x, p1.y}) - image.get({p1.x, p0.y}) +
            image.get({p0.x, p0.y});
    }

    inline T getInRange(Vector2I center, int range) const {
        return get({std::max(0, center.x - range), std::max(0, center.y - range)},
                {std::min(sizeX(), center.x + range + 1), std::min(sizeY(), center.y + range + 1)});
    }

    inline int sizeX() const { return image.sizeX() - 1; }
    inline int sizeY() const { return image.sizeY() - 1; }
};

template<typename T> inline std::ostream & operator<<(std::ostream &os, const IntegralImage<T> &integral) {
    for(int y = 0; y < integral.sizeY(); ++y) {
        for(int x = 0; x < integral.sizeX(); ++x) {
            os << integral.getInRange({x, y}, 0) << " ";
        }
        os << std::endl;
    }
    os << std::endl;
    return os;
}

#endif
