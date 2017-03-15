#ifndef VMBCAMERA_H
#define VMBCAMERA_H

// Vimba SDK dependency
#include <VimbaC.h>
// Qt dependencies
#include <QObject>
#include <QImage>

// Class to access a Gbe camera through Vimba SDK
// and use a Qt signal to inform about new images
class VmbCamera : public QObject {
	// Macro to use Qt signals
	Q_OBJECT
	// Public members
	public :
		// Construtor
		VmbCamera();
		// Destructor
		~VmbCamera();
		// Start the acquisition
		void StartCapture() const;
		// Stop the acquisition
		void StopCapture() const;
		// Get the camera exposure value (in microseconds)
		double Exposure() const;
		// Set the camera exposure value (in microseconds)
		void SetExposure( double exposure ) const;
		// Get the camera image
		const QImage* Image() const;
	// Qt signal
	signals :
		// Signal to tell a new image is received
		void ImageReady();
	// Private members
	private :
		// Handle to the camera
		VmbHandle_t handle;
		// Frame buffer size
		static const int frame_buffer_size;
		// Frame buffer
		VmbFrame_t* frame_buffer;
		// Current camera image
		QImage* image;
		// Vimba error
		VmbError_t error;
		// Function called by Vimba to receive the frame
		static void VMB_CALL FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer );
};

#endif // VMBCAMERA_H
