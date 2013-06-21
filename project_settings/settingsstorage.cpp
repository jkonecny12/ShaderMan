#include "settingsstorage.h"

/**
 * @brief SettingsStorage::SettingsStorage Create storage class for project settings.
 * @param parent Parent of this class (project).
 */
SettingsStorage::SettingsStorage(QObject *parent) :
    QObject(parent)
{
    blendFuncString << "ZERO" << "ONE" << "DST_COLOR" << "SRC_COLOR" << "ONE_MINUS_DST_COLOR" << "ONE_MINUS_SRC_COLOR" <<
                       "SRC_ALPHA" << "ONE_MINUS_SRC_ALPHA" << "DST_ALPHA" << "ONE_MINUS_DST_ALPHA" <<
                       "SRC_ALPHA_SATURATE";

    blending = false;

    backgroundColor[0] = 0.0;
    backgroundColor[1] = 0.0;
    backgroundColor[2] = 0.0;
    backgroundColor[3] = 1.0;

    depthTest = true;
}

/**
 * @brief SettingsStorage::getBlendFuncStrings Get enum BLEND_FUNCTION as list of strings with the same position like enum.
 * @return List of strings with blending functions.
 */
QStringList SettingsStorage::getBlendFuncStrings() const
{
    return blendFuncString;
}

/**
 * @brief SettingsStorage::setBlending Enable or disable blending.
 * @param blending If true blending will be enabled.
 */
void SettingsStorage::setBlending(bool blending)
{
    this->blending = blending;
}

/**
 * @brief SettingsStorage::setBlendFunc Set blending source factor and destination factor for OpenGL.
 * @param sFactor Source factor.
 * @param dFactor Destination factor.
 */
void SettingsStorage::setBlendFunc(SettingsStorage::BLEND_FUNCTION sFactor, BLEND_FUNCTION dFactor)
{
    blendFuncSFactor = sFactor;
    blendFuncDFactor = dFactor;
}

/**
 * @brief SettingsStorage::getBlendSFactor Get source factor for OpenGL blending function.
 * @return Source factor.
 */
SettingsStorage::BLEND_FUNCTION SettingsStorage::getBlendSFactor() const
{
    return blendFuncSFactor;
}

/**
 * @brief SettingsStorage::getBlendDFactor Get destination factor for OpenGL blending functions.
 * @return Destination factor.
 */
SettingsStorage::BLEND_FUNCTION SettingsStorage::getBlendDFactor() const
{
    return blendFuncDFactor;
}

/**
 * @brief SettingsStorage::isBlending Test if blending is enabled.
 * @return True if user set blending to enable, false if disable.
 */
bool SettingsStorage::isBlending() const
{
    return blending;
}

/**
 * @brief SettingsStorage::setBackgroundColor Set OpenGL background clearing color.
 * @param R Red color.
 * @param G Green color.
 * @param B Blue color.
 * @param A Alpha part.
 */
void SettingsStorage::setBackgroundColor(float R, float G, float B, float A)
{
    this->backgroundColor[0] = R;
    this->backgroundColor[1] = G;
    this->backgroundColor[2] = B;
    this->backgroundColor[3] = A;
}

/**
 * @brief SettingsStorage::getBackgroundColors Get colors OpenGL background clearing colors.
 * This array contains 4 numbers with R,G,B,A order.
 * @return Get 4 colors for clearing OpenGL window.
 */
const float *SettingsStorage::getBackgroundColors() const
{
    return backgroundColor;
}

/**
 * @brief SettingsStorage::setDepthTest Set if depth test will be enabled or disabled.
 * @param isDepth If true depth test will be enabled.
 */
void SettingsStorage::setDepthTest(bool isDepth)
{
    this->depthTest = isDepth;
}

/**
 * @brief SettingsStorage::isDepthTest Test if depth test will be enabled or disabled.
 * @return True if user want depth test, false otherwise.
 */
bool SettingsStorage::isDepthTest() const
{
    return this->depthTest;
}

/**
 * @brief operator << Operator for serialization of this class.
 * @param stream Where to serialize this.
 * @param settings What to serialize.
 * @return Stream for another use.
 */
QDataStream &operator <<(QDataStream &stream, const SettingsStorage *settings)
{
    // save clear colors
    const float *colors = settings->backgroundColor;

    qreal c = colors[0];
    stream << c;
    c = colors[1];
    stream << c;
    c = colors[2];
    stream << c;
    c = colors[3];
    stream << c;

    // save blending
    stream << settings->blending;

    qint32 i;

    i = settings->blendFuncSFactor;
    stream << i;
    i = settings->blendFuncDFactor;
    stream << i;



    return stream;
}

/**
 * @brief operator >> Operator for deserialization of this class.
 * @param stream Where to serialize.
 * @param settings What to serialize.
 * @return Stream for another use.
 */
QDataStream &operator >>(QDataStream &stream, SettingsStorage *&settings)
{
    settings = new SettingsStorage();

    // load clear colors
    qreal cR;
    stream >> cR;

    qreal cG;
    stream >> cG;

    qreal cB;
    stream >> cB;

    qreal cA;
    stream >> cA;

    settings->setBackgroundColor(cR, cG, cB, cA);

    // load blendings
    stream >> settings->blending;

    qint32 i;


    stream >> i;
    settings->blendFuncSFactor = static_cast<SettingsStorage::BLEND_FUNCTION>(i);

    stream >> i;
    settings->blendFuncDFactor = static_cast<SettingsStorage::BLEND_FUNCTION>(i);

    return stream;
}
