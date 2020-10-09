#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>

class Utils
{
public:
    static QPixmap ChangeHSL(QPixmap pix, float huerotate, float satadjust, float lumadjust);
};

#endif // UTILS_H
