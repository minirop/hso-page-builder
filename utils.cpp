#include "utils.h"
#include <QDebug>
#include <cmath>
using std::min, std::max;

struct Vec3 {
    float x = 0, y = 0, z = 0;

};

Vec3 operator+(const Vec3 & x, const Vec3 & y)
{
    return {
        x.x + y.x,
        x.y + y.y,
        x.z + y.z
    };
}

static Vec3 rgb_to_hsl(Vec3 color)
{
    Vec3 hsl;

    float fmin = min(min(color.x, color.y), color.z);
    float fmax = max(max(color.x, color.y), color.z);
    float delta = fmax - fmin;

    hsl.z = (fmax + fmin) / 2.0;

    if (delta == 0.0)
    {
        hsl.x = 0.0;
        hsl.y = 0.0;
    }
    else
    {
        if (hsl.z < 0.5)
            hsl.y = delta / (fmax + fmin);
        else
            hsl.y = delta / (2.0 - fmax - fmin);

        float dR = (((fmax - color.x) / 6.0) + (delta / 2.0)) / delta;
        float dG = (((fmax - color.y) / 6.0) + (delta / 2.0)) / delta;
        float dB = (((fmax - color.z) / 6.0) + (delta / 2.0)) / delta;

        if (color.x == fmax)
            hsl.x = dB - dG;
        else if (color.y == fmax)
            hsl.x = (1.0 / 3.0) + dR - dB;
        else if (color.z == fmax)
            hsl.x = (2.0 / 3.0) + dG - dR;

        if (hsl.x < 0.0)
            hsl.x += 1.0;
        else if (hsl.x > 1.0)
            hsl.x -= 1.0;
    }

    return hsl;
}

static float hue_to_rgb(float f1, float f2, float hue)
{
    if (hue < 0.0)
        hue += 1.0;
    else if (hue > 1.0)
        hue -= 1.0;

    float ret;

    if ((6.0 * hue) < 1.0)
        ret = f1 + (f2 - f1) * 6.0 * hue;
    else if ((2.0 * hue) < 1.0)
        ret = f2;
    else if ((3.0 * hue) < 2.0)
        ret = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
    else
        ret = f1;

    return ret;
}

static Vec3 hsl_to_rgb(Vec3 hsl)
{
    Vec3 rgb { hsl.z, hsl.z, hsl.z };

    if (hsl.y != 0.0)
    {
        float f2;

        if (hsl.z < 0.5)
            f2 = hsl.z * (1.0 + hsl.y);
        else
            f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);

        float f1 = 2.0 * hsl.z - f2;

        rgb.x = hue_to_rgb(f1, f2, hsl.x + (1.0 / 3.0));
        rgb.y = hue_to_rgb(f1, f2, hsl.x);
        rgb.z = hue_to_rgb(f1, f2, hsl.x - (1.0 / 3.0));
    }

    rgb.x = std::clamp(rgb.x, 0.f, 1.f);
    rgb.y = std::clamp(rgb.y, 0.f, 1.f);
    rgb.z = std::clamp(rgb.z, 0.f, 1.f);

    return rgb;
}

QPixmap Utils::ChangeHSL(QPixmap pix, float huerotate, float satadjust, float lumadjust)
{
    auto image = pix.toImage();

    for (int y = 0; y < pix.height(); y++)
    {
        for (int x = 0; x < pix.width(); x++)
        {
            auto color = image.pixelColor(x, y);
            if (color.alphaF() > 0)
            {
                Vec3 front = { (float)color.redF(), (float)color.greenF(), (float)color.blueF() };
                Vec3 rgb = rgb_to_hsl(front) + Vec3 { huerotate, 0, (lumadjust - 1.0f) * (float)color.alphaF() };
                rgb.y *= satadjust;
                rgb = hsl_to_rgb(rgb);
                image.setPixelColor(x, y, QColor(rgb.x * 255, rgb.y * 255, rgb.z * 255, color.alpha()));
            }
        }
    }

    return QPixmap::fromImage(image);
}
