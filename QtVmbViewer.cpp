#include "QtVmbViewer.h"
#include <iostream>
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
    connect( camera, &VmbCamera::FrameReceived, this, &QtVmbViewer::GetFrame );
    // Open the camera
    camera->Open();
    // Create the image
    image = new QImage( camera->width, camera->height, QImage::Format_Indexed8 );
    // Create a indexed color table
    image->setColorCount(256);
    for( int i = 0; i < 256; i++ ) {
        image->setColor( i, qRgb(i, i, i) );
    }
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
    // Delete the image
    delete image;
}

// Image callback
void QtVmbViewer::GetFrame( const VmbFrame_t* frame_pointer ) {
    // Copy the camera frame to the widget image
    VmbUchar_t* pCursor = image->bits();
    VmbUchar_t* pInBuffer = (VmbUchar_t*)frame_pointer->buffer;
    for( int y=0; y<camera->height; ++y ) {
        pCursor = image->scanLine( y );
        for( int x=0; x<camera->width; ++x ) {
            *pCursor = *pInBuffer;
            ++pCursor;
            ++pInBuffer;
        }
    }
    // Set the image to the label
    label->setPixmap( QPixmap::fromImage( *image ) );
    // Update the widget
    label->update();
}

