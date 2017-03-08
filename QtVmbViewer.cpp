#include "QtVmbViewer.h"
#include <QPixmap>
#include <cstdio>

// Camera identification number
static const char* camera_id =  "50-0503323406";

// Setup the frame buffer size
static const int frame_buffer_size = 10;

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
	// List the cameras
//	ListCameras();
	// Query all static details of the first known camera
	VmbCameraInfo_t camera;
	VmbUint32_t count;
	VmbCamerasList( &camera, 1, &count, sizeof(VmbCameraInfo_t) );
	// Connect to the camera
//	VmbCameraOpen( camera_id, VmbAccessModeFull, &handle );
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

// List the cameras
const char* QtVmbViewer::ListCameras() {
	VmbError_t          err             = VmbErrorSuccess;
	VmbCameraInfo_t *   pCameras        = NULL;
	VmbUint32_t         i               = 0;
	VmbUint32_t         nCount          = 0;
	VmbUint32_t         nFoundCount     = 0;
	// Get the amount of known cameras
	err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );
	if( VmbErrorSuccess == err && nCount != 0 ) {
		printf( "Cameras found: %d\n\n", nCount );
		pCameras = (VmbCameraInfo_t*)malloc( sizeof *pCameras * nCount );
		if( NULL != pCameras ) {
			// Query all static details of all known cameras without having to open the cameras
			err = VmbCamerasList( pCameras, nCount, &nFoundCount, sizeof *pCameras );
			if( VmbErrorSuccess == err ||  VmbErrorMoreData == err ) {
				// If a new camera was connected since we queried
				// the amount of cameras, we can ignore that one
				if( nFoundCount < nCount) {
					nCount = nFoundCount;
				}
				// And print them out
				for ( i=0; i<nCount; ++i ) {
					printf( "/// Camera Name: %s\n/// Model Name: %s\n/// Camera ID: %s\n/// Serial Number: %s\n/// @ Interface ID: %s\n\n\n",
						pCameras[i].cameraName,
						pCameras[i].modelName,
						pCameras[i].cameraIdString,
						pCameras[i].serialString,
						pCameras[i].interfaceIdString );
				}
				// Use the first camera
				if( nFoundCount != 0) {
					return pCameras[0].cameraIdString;
				}
			}
			else {
				printf( "Could not retrieve camera list. Error code: %d\n", err );
			}
			free( pCameras );
			pCameras = NULL;
		}
		else {
			printf( "Could not allocate camera list.\n" );
		}
	}
	else {
		printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
	}
	return NULL;
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
