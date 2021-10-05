#ifndef TRANSFORMSETTINGS_H
#define TRANSFORMSETTINGS_H

enum eTransformRotateType
{
    None = 0,
    Deg90CounterClockwise = 1,
    Deg180 = 2,
    Deg90Clockwise = 3,
    Horizontal = 4,
    UpperRightDiagonal = 5,
    Vertical = 6,
    UpperLeftDiagonal = 7
};

struct TransformSettings
{
public:
    TransformSettings();

    int cropLeft = 0;
    int cropTop = 0;
    int cropRight = 0;
    int cropBottom = 0;

    bool isResizeActivated = false;
    int resizeWidth = 0;
    int resizeHeight = 0;

    eTransformRotateType rotateType = eTransformRotateType::None;
};

#endif // TRANSFORMSETTINGS_H
