#ifndef QTVMBVIEWER_H
#define QTVMBVIEWER_H

#include <QWidget>
#include <QLabel>
#include "VmbCamera.h"

// Qt widget to display the images from an Allied Vision camera
class QtVmbViewer : public QWidget {

    Q_OBJECT

    // Public members
    public :
        // Constructor
        QtVmbViewer(QWidget *parent = 0);
        // Destructor
        ~QtVmbViewer();

        // Label to display the camera image
        QLabel* label;
        // Allied Vision camera
        VmbCamera* camera;

    // Qt slots
    public slots :
        // Slot to receive the image from the camera and update the widget
        void UpdateImage();
};

#endif // QTVMBVIEWER_H
