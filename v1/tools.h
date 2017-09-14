#ifndef TOOLS_H
#define TOOLS_H

#include<QImage>
#include<QDebug>
#include<QPainter>

namespace Tools {
    QImage Warm(int delta,QImage origin);
    QImage Cool(int delta,QImage origin);
    QImage Brightness(int delta,QImage origin);
}
#endif // TOOLS_H
