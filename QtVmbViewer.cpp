#include "QtVmbViewer.h"
#include <QPixmap>
#include <QVBoxLayout>

// Constructor
QtVmbViewer::QtVmbViewer( QWidget *parent ) : QWidget( parent ),
	label( new QLabel ), slider( new QSlider(Qt::Horizontal) ), camera( new VmbCamera ) {
	// Scale the image label
	label->setScaledContents( true );
	// Widget layout
	QVBoxLayout* layout = new QVBoxLayout( this );
	layout->addWidget( label );
	layout->addWidget( slider );
	layout->setSizeConstraint( QLayout::SetFixedSize );
	// Setup the slider according the camera exposure parameters
	slider->setRange( 64, 10000 );
	slider->setValue( camera->Exposure() );
	slider->update();
	// Connect the camera signal to get newly received images
	connect( camera, &VmbCamera::ImageReady, this, &QtVmbViewer::UpdateImage );
	// Connect the slider released signal to set the new camera exposure value
	connect( slider, &QSlider::sliderReleased, this, &QtVmbViewer::SetExposure );
	// Start acquisition
	camera->StartCapture();
}

// Destructor
QtVmbViewer::~QtVmbViewer() {
	// Stop acquisition
	camera->StopCapture();
	// Delete the camera
	delete camera;
}

// Slot to get a new image from the camera and update the widget
void QtVmbViewer::UpdateImage() {
	// Set the image to the label
	label->setPixmap( QPixmap::fromImage( *camera->Image() ) );
	// Scale the label
	label->resize( 0.3 * label->pixmap()->size() );
	// Update the widget
	label->update();
}

// Slot to update the camera exposure
void QtVmbViewer::SetExposure() {
	camera->SetExposure( slider->value() );
}
