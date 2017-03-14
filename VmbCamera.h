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

		// Start the acquisition
		void StartCapture() const;
		// Stop the acquisition
		void StopCapture() const;

		// Get the camera exposure value (in microseconds)
		double Exposure() const;
		// Set the camera exposure value (in microseconds)
		void SetExposure( double exposure ) const;

		// Get the camera image
		const QImage* Image() const {
			return image;
		}

	// Private members
	private :
		// Handle to the camera
		VmbHandle_t handle;
		// Frame buffer size
		static const int frame_buffer_size = 10;
		// Frame buffer
		VmbFrame_t* frame_buffer;
		// Current camera image
		QImage* image;

		// Function called by Vimba to receive the frame
		static void VMB_CALL FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer );

	// Qt signals
	signals :
		// Signal to tell a new image is received
		void ImageReady();
};

#endif // VMBCAMERA_H
