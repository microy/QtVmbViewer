#include "QtVmbViewer.h"
#include <QPixmap>
#include <QVBoxLayout>

// Constructor
QtVmbViewer::QtVmbViewer( QWidget *parent ) : QWidget( parent ), label( new QLabel ) {
    // Scale the image label
    label->setScaledContents( true );
    // Widget layout
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->addWidget( label );
    layout->setSizeConstraint( QLayout::SetFixedSize );
    // Create the camera
    camera = new VmbCamera( "50-0503323406" );
    // Connect the camera signal to get the received frame
    connect( camera, &VmbCamera::ImageReady, this, &QtVmbViewer::UpdateImage );
    // Open the camera
    camera->Open();
    // Start acquisition
    camera->StartCapture();
}

// Destructor
QtVmbViewer::~QtVmbViewer() {
    // Stop acquisition
    camera->StopCapture();
    // Close the camera
    camera->Close();
    // Delete the camera
    delete camera;
}

// Image callback
void QtVmbViewer::UpdateImage() {
    // Set the image to the label
    label->setPixmap( QPixmap::fromImage( *camera->image ) );
    // Update the widget
    label->update();
}

