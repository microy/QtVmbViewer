#include "QtVmbViewer.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <iostream>

// Constructor
QtVmbViewer::QtVmbViewer( QWidget *parent ) : QWidget( parent ),
	label( new QLabel ), slider( new QSlider(Qt::Horizontal) ) {
	// Scale the image label
	label->setScaledContents( true );
	// Widget layout
	QVBoxLayout* layout = new QVBoxLayout( this );
	layout->addWidget( label );
	layout->addWidget( slider );
	layout->setSizeConstraint( QLayout::SetFixedSize );
	// Create the camera
	camera = new VmbCamera( "50-0503323406" );
	// Connect the camera signal to get newly received images
	connect( camera, &VmbCamera::ImageReady, this, &QtVmbViewer::UpdateImage );
	// Open the camera
	camera->Open();
	// Setup the slider according the camera exposure parameters
	slider->setRange( camera->exposure_min, camera->exposure_max );
	slider->setValue( camera->exposure );
	slider->update();
	// Connect the slider released signal to set the new camera exposure value
	connect( slider, &QSlider::sliderReleased, this, &QtVmbViewer::SetExposure );
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

// Slot to get a new image from the camera and update the widget
void QtVmbViewer::UpdateImage() {
	// Set the image to the label
	label->setPixmap( QPixmap::fromImage( *camera->image ).scaled( camera->width*0.3, camera->height*0.3, Qt::KeepAspectRatio ) );
	// Update the widget
	label->update();
}

// Slot to update the camera exposure
void QtVmbViewer::SetExposure() {
	camera->SetExposure( slider->value() );
}
