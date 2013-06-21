#include "assimpprogresshandler.h"
#include <QDebug>

/**
 * @brief AssimpProgressHandler::AssimpProgressHandler Create assimp handler and get progress dialog.
 * @param dialog Created progress dialog.
 */
AssimpProgressHandler::AssimpProgressHandler(QProgressDialog *dialog) :
    Assimp::ProgressHandler()
{
    this->pd = dialog;
}

/**
 * @brief AssimpProgressHandler::Update method where we get percentage of loaded model and set this value to progress dialog.
 * @param percentage Percentage of loaded model.
 * @return If false is returned then assimp cancel loading of model when it can be canceled.
 */
bool AssimpProgressHandler::Update(float percentage)
{
    qDebug() << percentage;

    if(pd->wasCanceled())
        return false;

    int convert = percentage * 100;
    pd->setValue(convert);

    return true;
}
