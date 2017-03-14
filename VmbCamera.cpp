#include "VmbCamera.h"

// Construtor
VmbCamera::VmbCamera() {
	// Initialize Vimba
	VmbStartup();
	// Send discovery packet to GigE cameras
	VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce" );
	// Get the first known camera
	VmbCameraInfo_t camera;
	VmbUint32_t count;
	VmbCamerasList( &camera, 1, &count, sizeof(VmbCameraInfo_t) );
	// Connect to the camera
	VmbCameraOpen( camera.cameraIdString, VmbAccessModeFull, &handle );
	// Adjust packet size automatically
	VmbFeatureCommandRun( handle, "GVSPAdjustPacketSize" );
	// Get image parameters
	VmbInt64_t width;
	VmbInt64_t height;
	VmbInt64_t payload_size;
	VmbFeatureIntGet( handle, "Width", &width );
	VmbFeatureIntGet( handle, "Height", &height );
	VmbFeatureIntGet( handle, "PayloadSize", &payload_size );
	// Create the Qt image (grayscale)
	image = new QImage( width, height, QImage::Format_Indexed8 );
	// Create an indexed color table for the Qt image
	image->setColorCount( 256 );
	for( int i = 0; i < 256; i++ ) {
		image->setColor( i, qRgb(i, i, i) );
	}
	// Initialize the frame buffer
	frame_buffer = (VmbFrame_t*)malloc( frame_buffer_size * sizeof(VmbFrame_t) );
	// Allocate the frames
	for( int i=0; i<frame_buffer_size ; i++ ) {
		// Allocate accordingly
		frame_buffer[i].buffer = (VmbUchar_t*)malloc( (VmbUint32_t)payload_size );
		frame_buffer[i].bufferSize = (VmbUint32_t)payload_size;
		// Register this object in the frame context
		frame_buffer[i].context[0] = this;
	}
}

// Destructor
VmbCamera::~VmbCamera() {
	// Close the camera
	VmbCameraClose( handle );
	// Shutdown Vimba
	VmbShutdown();
	// Free the frame buffer
	for( int i=0; i<frame_buffer_size ; i++ ) {
		free( frame_buffer[i].buffer );
	}
	free( frame_buffer );
	// Delete the Qt image
	delete image;
}

// Start acquisition
void VmbCamera::StartCapture() {
	// Announce the frames
	for( int i=0; i<frame_buffer_size ; i++ ) {
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
void VmbCamera::StopCapture() {
	// Stop acquisition
	VmbFeatureCommandRun( handle, "AcquisitionStop" );
	// Flush the frame queue
	VmbCaptureQueueFlush( handle );
	// Stop capture engine
	VmbCaptureEnd( handle );
	// Revoke the frames
	VmbFrameRevokeAll( handle );
}

// Get the camera exposure time in microseconds
double VmbCamera::Exposure() const {
	double exposure;
	VmbFeatureFloatGet( handle, "ExposureTimeAbs", &exposure );
	return exposure;
}

// Set the camera exposure time in microseconds
void VmbCamera::SetExposure( double exposure ) const {
	VmbFeatureFloatSet( handle, "ExposureTimeAbs", exposure );
}

// The callback that gets executed on every filled frame
void VMB_CALL VmbCamera::FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer ) {
	// Check frame status
	if( frame_pointer->receiveStatus == VmbFrameStatusComplete ) {
		// Get the camera object in the frame context
		VmbCamera* camera = (VmbCamera*)frame_pointer->context[0];
		// Copy the camera frame buffer to the Qt image
		memcpy( camera->image->bits(), frame_pointer->buffer, frame_pointer->bufferSize );
		// Emit the image ready signal
		emit camera->ImageReady();
	}
	// Requeue the frame
	VmbCaptureFrameQueue( camera_handle , frame_pointer , &FrameDoneCallback );
}
