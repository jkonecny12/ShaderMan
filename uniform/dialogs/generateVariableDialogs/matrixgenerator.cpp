#include "matrixgenerator.h"

/**
 * @brief MatrixGenerator::getRotXWithButtons Get generated rotation matrix 4x4 around X axis.
 * @param button With what button we want control this transformation.
 * @param startAngle Start with this angle.
 * @param speed Time between incrementing/decrementing values in button variables.
 * @return Created matrix as list of strings.
 */
QStringList MatrixGenerator::getRotXWithButtons(AXIS button, float startAngle, ulong speed)
{
    QStringList list;

    int buttonP = 0;
    int buttonM = 0;

    setButtonVariables(button, buttonM, buttonP);
    list <<
            // first row
            "1" <<
            "0" <<
            "0" <<
            "0" <<

            // second row
            "0" <<
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            QString("sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<

            // third row
            "0" <<
            QString("-sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<

            //fourth row
            "0" <<
            "0" <<
            "0" <<
            "1";

    return list;
}

/**
 * @brief MatrixGenerator::getRotYWithButtons Get generated rotation matrix 4x4 around Y axis.
 * @param button With what button we want control this transformation.
 * @param startAngle Start with this angle.
 * @param speed Time between incrementing/decrementing values in button variables.
 * @return Created matrix as list of strings.
 */
QStringList MatrixGenerator::getRotYWithButtons(MatrixGenerator::AXIS button, float startAngle, ulong speed)
{
    QStringList list;

    int buttonP = 0;
    int buttonM = 0;


    setButtonVariables(button, buttonM, buttonP);

    list <<
            // first row
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<
            QString("-sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<

            // second row
            "0" <<
            "1" <<
            "0" <<
            "0" <<

            // third row
            QString("sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<

            //fourth row
            "0" <<
            "0" <<
            "0" <<
            "1";


    return list;
}

/**
 * @brief MatrixGenerator::getRotZWithButtons Get generated rotation matrix 4x4 around Z axis.
 * @param button With what button we want control this transformation.
 * @param startAngle Start with this angle.
 * @param speed Time between incrementing/decrementing values in button variables.
 * @return Created matrix as list of strings.
 */
QStringList MatrixGenerator::getRotZWithButtons(MatrixGenerator::AXIS button, float startAngle, ulong speed)
{
    QStringList list;

    int buttonP = 0;
    int buttonM = 0;

    setButtonVariables(button, buttonM, buttonP);

    list <<
            // first row
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            QString("sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<
            "0" <<

            // second row
            QString("-sin( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            QString("cos( ($ActionPressed{%2,%1,1,0} - $ActionPressed{%3,%1,1,0} + (%4)) * (pi/180) )")
                .arg(speed).arg(buttonM).arg(buttonP).arg(startAngle) <<
            "0" <<
            "0" <<

            // third row
            "0" <<
            "0" <<
            "1" <<
            "0" <<

            //fourth row
            "0" <<
            "0" <<
            "0" <<
            "1";

    return list;
}

/**
 * @brief MatrixGenerator::getTranslation Get translation matrix with or without using control buttons.
 * @param start Start position for this matrix transformation.
 * @param xButton True if we want use X+ and X- buttons for control this transformation in X axis translation.
 * @param yButton True if we want use Y+ and Y- buttons for control this transformation in Y axis translation.
 * @param zButton True if we want use Z+ and Z- buttons for control this transformation in Z axis translation.
 * @param speed Time between incrementing/decrementing values in button variables.
 * @return Created matrix as list of strings.
 */
QStringList MatrixGenerator::getTranslation(QVector3D start, bool xButton, bool yButton, bool zButton, ulong speed)
{
    QStringList list;
    QString x, y, z;

    if(xButton)
        x = QString("$ActionPressed{10,%1,1,0} - $ActionPressed{9,%1,1,0} + (%2)").arg(speed).arg(start.x());
    else
        x = QString("%1").arg(start.x());

    if(yButton)
        y = QString("$ActionPressed{12,%1,1,0} - $ActionPressed{11,%1,1,0} + (%2)").arg(speed).arg(start.y());
    else
        y = QString("%1").arg(start.y());

    if(zButton)
        z = QString("$ActionPressed{14,%1,1,0} - $ActionPressed{13,%1,1,0} + (%2)").arg(speed).arg(start.z());
    else
        z = QString("%1").arg(start.z());

    list <<
            // first row
            "1" <<
            "0" <<
            "0" <<
            x <<

            // second row
            "0" <<
            "1" <<
            "0" <<
            y <<

            // third row
            "0" <<
            "0" <<
            "1" <<
            z <<

            //fourth row
            "0" <<
            "0" <<
            "0" <<
            "1";

    return list;
}

/**
 * @brief MatrixGenerator::MatrixGenerator This class canno't be created (private), use only static methods.
 */
MatrixGenerator::MatrixGenerator()
{
}
