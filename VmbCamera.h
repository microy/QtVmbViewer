#ifndef VMBCAMERA_H
#define VMBCAMERA_H

// Standard dependency
#include <string>
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
		VmbCamera( const char* camera_id, int frame_buffer_size=10 );
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
		// Function called by Vimba to receive the frame
		static void VMB_CALL FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer );

		// Handle to the camera
		VmbHandle_t handle;
		// Identification number of the camera
		std::string id;
		// Image parameters
		VmbInt64_t width;
		VmbInt64_t height;
		VmbInt64_t payloadsize;
		// Frame buffer
		int frame_buffer_size;
		VmbFrame_t* frame_buffer;
		// Current camera image
		QImage* image;

	// Qt signals
	signals :
		// Signal to tell a new image is received
		void ImageReady();
};

#endif // VMBCAMERA_H
