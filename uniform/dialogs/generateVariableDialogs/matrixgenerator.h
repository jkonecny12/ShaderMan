#ifndef MATRIXGENERATOR_H
#define MATRIXGENERATOR_H

#include <QStringList>
#include <QVector3D>

/**
 * @brief The MatrixGenerator class Generate matrices as StringLists. Using static methods.
 */
class MatrixGenerator
{
public:
    enum AXIS {X, Y, Z};

    static QStringList getRotXWithButtons(AXIS button, float startAngle = 0, ulong speed = 50);
    static QStringList getRotYWithButtons(AXIS button, float startAngle = 0, ulong speed = 50);
    static QStringList getRotZWithButtons(AXIS button, float startAngle = 0, ulong speed = 50);

    static QStringList getTranslation(QVector3D start, bool xButton = false, bool yButton = false,
                                      bool zButton = false, ulong speed = 50);

private:
    MatrixGenerator();

    /**
     * @brief setButtonVariables Set button variables for default buttons.
     * @param axis Axis what user want.
     * @param buttonM Set id to this minus button.
     * @param buttonP Set id to this plus button.
     */
    inline static void setButtonVariables(AXIS axis, int &buttonM, int &buttonP)
    {
        switch(axis)
        {
        case X:
            buttonP = 10;
            buttonM = 9;
            break;
        case Y:
            buttonP = 12;
            buttonM = 11;
            break;
        case Z:
            buttonP = 14;
            buttonM = 13;
            break;
        }
    }

};

#endif // MATRIXGENERATOR_H
