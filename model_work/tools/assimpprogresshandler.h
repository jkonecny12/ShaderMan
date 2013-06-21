#ifndef ASSIMPPROGRESSHANDLER_H
#define ASSIMPPROGRESSHANDLER_H

#include <assimp/ProgressHandler.hpp>
#include <QProgressDialog>

/**
 * @brief The AssimpProgressHandler class using QProgressDialog for setting value.
 * Getting progress from assimp importer library and setting this to QProgressDialog.
 */
class AssimpProgressHandler : public Assimp::ProgressHandler
{
public:
    AssimpProgressHandler(QProgressDialog* dialog);

    bool Update(float percentage = -1.f);

private:
    QProgressDialog* pd;
};

#endif // ASSIMPPROGRESSHANDLER_H
