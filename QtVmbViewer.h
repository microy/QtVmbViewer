#ifndef QTVMBVIEWER_H
#define QTVMBVIEWER_H

#include <QWidget>
#include <QImage>
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
        // The camera image
        QImage* image;
        // Allied Vision camera
        VmbCamera* camera;

    // Public slot
    public slots :
        // Slot to receive the frame from the camera
        void GetFrame( const VmbFrame_t* frame_pointer );
};

#endif // QTVMBVIEWER_H
