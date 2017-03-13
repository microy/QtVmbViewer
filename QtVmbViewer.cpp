#include "QtVmbViewer.h"
#include <QPixmap>
#include <cstdio>

// Constructor
QtVmbViewer::QtVmbViewer( QWidget* parent ) : QLabel( parent ) {
	// Connect the ImageReady signal with the UpdateImage slot to get newly received images
	connect( this, &QtVmbViewer::ImageReady, this, &QtVmbViewer::UpdateImage );
	// Initialize Vimba
	VmbStartup();
	// Send discovery packet to GigE cameras
	VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce" );
}

// Destructor
QtVmbViewer::~QtVmbViewer() {
	// Shutdown Vimba
	VmbShutdown();
}

// Open the camera
void QtVmbViewer::Open() {
	// Query all static details of the first known camera
	VmbCameraInfo_t camera;
	VmbUint32_t count;
	VmbCamerasList( &camera, 1, &count, sizeof(VmbCameraInfo_t) );
	// Connect to the camera
	VmbCameraOpen( camera.cameraIdString, VmbAccessModeFull, &handle );
	// Adjust packet size automatically
	VmbFeatureCommandRun( handle, "GVSPAdjustPacketSize" );
	// Get image parameters
	VmbFeatureIntGet( handle, "Width", &width );
	VmbFeatureIntGet( handle, "Height", &height );
	VmbFeatureIntGet( handle, "PayloadSize", &payloadsize );
}

// Close the camera
void QtVmbViewer::Close() {
	// Close the camera
	VmbCameraClose( handle );
}

// Start acquisition
void QtVmbViewer::StartCapture() {
	// Create the Qt image (grayscale)
	image = new QImage( width, height, QImage::Format_Indexed8 );
	// Create an indexed color table for the QImage
	image->setColorCount( 256 );
	for( int i = 0; i < 256; i++ ) {
		image->setColor( i, qRgb(i, i, i) );
	}
	// Initialize the frame buffer
	frame_buffer = (VmbFrame_t*)malloc( frame_buffer_size * sizeof(VmbFrame_t) );
	// Allocate and announce the frames
	for( int i=0; i<frame_buffer_size ; i++ ) {
		// Allocate accordingly
		frame_buffer[i].buffer = (VmbUchar_t*)malloc( (VmbUint32_t)payloadsize );
		frame_buffer[i].bufferSize = (VmbUint32_t)payloadsize;
		// Register this object in the frame context
		frame_buffer[i].context[0] = this;
		// Announce the frame
		VmbFrameAnnounce( handle, &frame_buffer[i], sizeof( VmbFrame_t ) );
	}
	// Start capture engine
	VmbCaptureStart( handle );
	// Queue frames and register callback
	for( int i=0; i<frame_buffer_size; i++ ) {
		VmbCaptureFrameQueue( handle, &frame_buffer[i], &FrameDoneCallback );
	}
	// Start acquisition
	VmbFeatureCommandRun( handle, "AcquisitionStart" );
}

// Stop acquisition
void QtVmbViewer::StopCapture() {
	// Stop acquisition
	VmbFeatureCommandRun( handle, "AcquisitionStop" );
	// Flush the frame queue
	VmbCaptureQueueFlush( handle );
	// Stop capture engine
	VmbCaptureEnd( handle );
	// Revoke the frames
	VmbFrameRevokeAll( handle );
	// Free the frame buffer
	for( int i=0; i<frame_buffer_size ; i++ ) {
		free( frame_buffer[i].buffer );
	}
	free( frame_buffer );
	// Delete the image
	delete image;
}

// The callback that gets executed on every filled frame
void VMB_CALL QtVmbViewer::FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer ) {
	// Check frame status
	if( frame_pointer->receiveStatus == VmbFrameStatusComplete ) {
		// Get the camera viewer object in the frame context
		QtVmbViewer* viewer = (QtVmbViewer*)frame_pointer->context[0];
		// Copy the camera frame buffer to the Qt image
		memcpy( viewer->image->bits(), frame_pointer->buffer, frame_pointer->bufferSize );
		// Emit the image ready signal
		emit viewer->ImageReady();
	}
	// Requeue the frame
	VmbCaptureFrameQueue( camera_handle , frame_pointer , &FrameDoneCallback );
}

// Slot to get a new image from the camera and update the widget
void QtVmbViewer::UpdateImage() {
	// Set the image to the label
	setPixmap( QPixmap::fromImage( *image ).scaled( width*0.3, height*0.3, Qt::KeepAspectRatio ) );
	// Update the widget
	update();
}
