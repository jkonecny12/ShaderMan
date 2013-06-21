#ifndef SETTINGSSTORAGE_H
#define SETTINGSSTORAGE_H

#include <QObject>
#include <QStringList>

/**
 * @brief The SettingsStorage class for saving project settings (OpenGL settings included).
 */
class SettingsStorage : public QObject
{
    Q_OBJECT
public:
    enum BLEND_FUNCTION { ZERO = 0, ONE, DST_COLOR, SRC_COLOR, ONE_MINUS_DST_COLOR, ONE_MINUS_SRC_COLOR, SRC_ALPHA,
                         ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, SRC_ALPHA_SATURATE };
    explicit SettingsStorage(QObject *parent = 0);

    // blending part
    QStringList getBlendFuncStrings() const;

    void setBlending(bool blending);
    void setBlendFunc(BLEND_FUNCTION sFactor, BLEND_FUNCTION dFactor);

    BLEND_FUNCTION getBlendSFactor() const;
    BLEND_FUNCTION getBlendDFactor() const;

    bool isBlending() const;

    // background part
    void setBackgroundColor(float R, float G, float B, float A);
    const float *getBackgroundColors() const;

    // depth test
    void setDepthTest(bool isDepth);
    bool isDepthTest() const;


    friend QDataStream & operator<< (QDataStream& stream, const SettingsStorage* settings);
    friend QDataStream & operator>> (QDataStream& stream, SettingsStorage*& settings);

private:
    QStringList blendFuncString;
    BLEND_FUNCTION blendFuncSFactor;
    BLEND_FUNCTION blendFuncDFactor;
    bool blending;
    float backgroundColor[4];
    bool depthTest;
    
signals:
    
public slots:
    
};

QDataStream & operator<< (QDataStream& stream, const SettingsStorage* settings);
QDataStream & operator>> (QDataStream& stream, SettingsStorage*& settings);

#endif // SETTINGSSTORAGE_H
