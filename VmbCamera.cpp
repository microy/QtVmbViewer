#include "VmbCamera.h"
#include <iostream>

// Frame buffer size
const int VmbCamera::frame_buffer_size = 5;

// Construtor
VmbCamera::VmbCamera() {
	// Initialize Vimba
	error = VmbStartup();
	if( error != VmbErrorSuccess ) {
		std::cerr << "Could not start Vimba system. Error code: " << error << '\n';
	}
	// Send discovery packet to GigE cameras
	error = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce" );
	if( error != VmbErrorSuccess ) {
		std::cerr << "Could not ping GigE cameras over the network. Error code: " << error << '\n';
	}
	// Get the first known camera
	VmbCameraInfo_t camera;
	VmbUint32_t count;
	error = VmbCamerasList( &camera, 1, &count, sizeof(VmbCameraInfo_t) );
	if( (error != VmbErrorSuccess && error != VmbErrorMoreData) || count == 0 ) {
		std::cerr << "Could not list cameras or no camera present. Error code: " << error << '\n';
	}
	// Connect to the camera
	error = VmbCameraOpen( camera.cameraIdString, VmbAccessModeFull, &handle );
	if( error != VmbErrorSuccess ) {
		std::cerr << "Could not open camera. Error code: " << error << '\n';
	}
	// Adjust packet size automatically
	error = VmbFeatureCommandRun( handle, "GVSPAdjustPacketSize" );
	if( error != VmbErrorSuccess ) {
		std::cerr << "Could not adjust packet size. Error code: " << error << '\n';
	}
	// Get image parameters
	VmbInt64_t width;
	VmbInt64_t height;
	VmbInt64_t payload_size;
	VmbFeatureIntGet( handle, "Width", &width );
	VmbFeatureIntGet( handle, "Height", &height );
	VmbFeatureIntGet( handle, "PayloadSize", &payload_size );
	// Create the Qt image (grayscale)
	image = QImage( width, height, QImage::Format_Indexed8 );
	// Create an indexed color table for the Qt image
	image.setColorCount( 256 );
	for( int i=0; i<256; i++ ) {
		image.setColor( i, qRgb(i, i, i) );
	}
	// Initialize the frame buffer
	frame_buffer = (VmbFrame_t*)malloc( frame_buffer_size * sizeof(VmbFrame_t) );
	// Allocate the frames
	for( int i=0; i<frame_buffer_size; i++ ) {
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
	error = VmbCameraClose( handle );
	if( error != VmbErrorSuccess ) {
		std::cerr << "Could not close camera. Error code: " << error << '\n';
	}
	// Shutdown Vimba
	VmbShutdown();
	// Free the frame buffer
	for( int i=0; i<frame_buffer_size; i++ ) {
		free( frame_buffer[i].buffer );
	}
	free( frame_buffer );
}

// Start acquisition
void VmbCamera::StartCapture() const {
	// Announce the frames
	for( int i=0; i<frame_buffer_size; i++ ) {
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
void VmbCamera::StopCapture() const {
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

// Get the camera image
const QImage& VmbCamera::Image() const {
	return image;
}

// The callback that gets executed on every filled frame
void VMB_CALL VmbCamera::FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer ) {
	// Check frame status
	if( frame_pointer->receiveStatus == VmbFrameStatusComplete ) {
		// Get the camera object in the frame context
		VmbCamera* camera = (VmbCamera*)frame_pointer->context[0];
		// Copy the camera frame buffer to the Qt image
		memcpy( camera->image.bits(), frame_pointer->buffer, frame_pointer->bufferSize );
		// Emit the image ready signal
		emit camera->ImageReady();
	}
	// Requeue the frame
	VmbCaptureFrameQueue( camera_handle , frame_pointer , &FrameDoneCallback );
}
