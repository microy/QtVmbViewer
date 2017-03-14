#ifndef VMBCAMERA_H
#define VMBCAMERA_H

// Vimba SDK dependency
#include <VimbaC.h>
// Qt dependency
#include <QObject>
#include <QImage>

// Class to access a Gbe camera through Vimba SDK
class VmbCamera : public QObject {

	Q_OBJECT

	// Public members
	public :
		// Construtor
		VmbCamera();
		// Destructor
		~VmbCamera();

		// Open the camera
		void Open();
		// Close the camera
		void Close();
		// Start the acquisition
		void StartCapture();
		// Stop the acquisition
		void StopCapture();
		// Get the camera exposure value (in microseconds)
		double GetExposure();
		// Set the camera exposure value (in microseconds)
		void SetExposure( double exposure );
		// Function called by Vimba to receive the frame
		static void VMB_CALL FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer );

		// Handle to the camera
		VmbHandle_t handle;
		// Image parameters
		VmbInt64_t width;
		VmbInt64_t height;
		VmbInt64_t payloadsize;
		// Frame buffer size
		static const int frame_buffer_size = 10;
		// Frame buffer
		VmbFrame_t* frame_buffer;
		// Current camera image
		QImage* image;

	// Qt signals
	signals :
		// Signal to tell a new image is received
		void ImageReady();
};

#endif // VMBCAMERA_H
