#include "VmbCamera.h"
#include <cstdlib>

// Construtor
VmbCamera::VmbCamera( const char* camera_id ) : id( camera_id ) {
    // Initialize Vimba
    VmbStartup();
    // Send discovery packet to GigE cameras
    VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce" );
}

// Destructor
VmbCamera::~VmbCamera() {
    // Shutdown Vimba
    VmbShutdown();
}

// Open the camera
void VmbCamera::Open() {
    // Connect to the camera
    VmbCameraOpen( id.c_str(), VmbAccessModeFull, &handle );
    // Adjust packet size automatically
    VmbFeatureCommandRun( handle, "GVSPAdjustPacketSize" );
    // Get image parameters
    VmbFeatureIntGet( handle, "Width", &width );
    VmbFeatureIntGet( handle, "Height", &height );
    VmbFeatureIntGet( handle, "PayloadSize", &payloadsize );
    // Create the Qt image
    image = new QImage( width, height, QImage::Format_Indexed8 );
    // Create a indexed color table
    image->setColorCount( 256 );
    for( int i = 0; i < 256; i++ ) {
        image->setColor( i, qRgb(i, i, i) );
    }
}

// Close the camera
void VmbCamera::Close() {
    // Close the camera
    VmbCameraClose( handle );
    // Delete the image
    delete image;
}

// Start acquisition
void VmbCamera::StartCapture() {
    // Initialize the frame buffer
    for( int i=0; i<10 ; ++i ) {
        // Allocate accordingly
        frames[i].buffer = (VmbUchar_t*)malloc( (VmbUint32_t)payloadsize );
        frames[i].bufferSize = (VmbUint32_t)payloadsize;
        // Register this object in the frame context
        frames[i].context[0] = this;
        // Announce the frame
        VmbFrameAnnounce( handle, &frames[i], sizeof( VmbFrame_t ) );
    }
    // Start capture engine
    VmbCaptureStart( handle );
    // Queue frames and register callback
    for( int i=0; i<10; ++i ) {
        VmbCaptureFrameQueue( handle, &frames[i], &FrameDoneCallback );
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
    // Free the frame buffer
    for( int i=0; i<10 ; ++i ) {
        free( frames[i].buffer );
    }
}

// The callback that gets executed on every filled frame
void VMB_CALL VmbCamera::FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer ) {
    // Check frame status
    if( frame_pointer->receiveStatus == VmbFrameStatusComplete ) {
        // Get the camera object in the frame context
        VmbCamera* camera = (VmbCamera*)frame_pointer->context[0];
        // Copy the camera frame buffer to the Qt image
        memcpy( camera->image->bits(), frame_pointer->buffer, frame_pointer->bufferSize );
        // Emit the frame received signal
        emit camera->ImageReady();
    }
    // Requeue the frame
    VmbCaptureFrameQueue( camera_handle , frame_pointer , &FrameDoneCallback );
}
