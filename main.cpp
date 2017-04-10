#include <librealsense/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace rs;

// Window size and frame rate
int const INPUT_WIDTH 	= 320;
int const INPUT_HEIGHT 	= 240;
int const FRAMERATE 	= 60;

// Named windows
char* const WINDOW_DEPTH = "Depth Image";
char* const WINDOW_RGB	 = "RGB Image";
char* const WINDOW_DEPTH_ALIGN = "Align Depth Image";
char* const WINDOW_RGB_ALIGN = "Align RGB Image";


context 	_rs_ctx;
device* 	_rs_camera = NULL;
intrinsics 	_depth_intrin;
intrinsics  	_color_intrin;
intrinsics 	_depth_align_intrin;
intrinsics  	_color_align_intrin;
bool 		_loop = true;

//just need global varibles about rgb and aligned depth,maybe should be convert to 8 bit
//cv::Mat rgb;
//cv::Mat depth8u_align;

//filename flag
int filename_flag = 0;

// Initialize the application state. Upon success will return the static app_state vars address
bool initialize_streaming( )
{
	bool success = false;
	if( _rs_ctx.get_device_count( ) > 0 )
	{
		_rs_camera = _rs_ctx.get_device( 0 );

		_rs_camera->enable_stream( rs::stream::color, INPUT_WIDTH, INPUT_HEIGHT, rs::format::rgb8, FRAMERATE );
		_rs_camera->enable_stream( rs::stream::depth, INPUT_WIDTH, INPUT_HEIGHT, rs::format::z16, FRAMERATE );
		//_rs_camera->enable_stream( rs::stream::color_aligned_to_depth, INPUT_WIDTH, INPUT_HEIGHT, rs::format::rgb8, FRAMERATE );
		//_rs_camera->enable_stream( rs::stream::depth_aligned_to_color, INPUT_WIDTH, INPUT_HEIGHT, rs::format::z16, FRAMERATE );

		_rs_camera->start( );

		success = true;
	}
	return success;
}



/////////////////////////////////////////////////////////////////////////////
// If the left mouse button was clicked on either image, stop streaming and close windows.
/////////////////////////////////////////////////////////////////////////////
static void onMouse( int event, int x, int y, int, void* window_name )
{
	if( event == cv::EVENT_LBUTTONDOWN )
	{
		_loop = false;
	}
}



/////////////////////////////////////////////////////////////////////////////
// Create the depth and RGB windows, set their mouse callbacks.
// Required if we want to create a window and have the ability to use it in
// different functions
/////////////////////////////////////////////////////////////////////////////
void setup_windows( )
{
	cv::namedWindow( WINDOW_DEPTH, 0 );
	cv::namedWindow( WINDOW_RGB, 0 );
	cv::namedWindow( WINDOW_DEPTH_ALIGN, 0);
	cv::namedWindow( WINDOW_RGB_ALIGN, 0);
	

	cv::setMouseCallback( WINDOW_DEPTH, onMouse, WINDOW_DEPTH );
	cv::setMouseCallback( WINDOW_RGB, onMouse, WINDOW_RGB );
	cv::setMouseCallback( WINDOW_DEPTH_ALIGN, onMouse, WINDOW_DEPTH_ALIGN );
	cv::setMouseCallback( WINDOW_RGB_ALIGN, onMouse, WINDOW_RGB_ALIGN );
}


/*
 * read keyboard input, if get 's', save the RGB image and aligned depth image.
 */
void ifSaveImage(cv::Mat rgb,cv::Mat depth)
{
  cv::Mat _rgb = rgb;
  cv::Mat _depth = depth;
  char c = cv::waitKey(33);
  if(c=='s' || c=='S')
  {
    filename_flag++;
    cout << filename_flag << endl;
    //"+to_string(filename_flag)+"
    cv::imwrite("img/rgb_"+to_string(filename_flag)+".jpg", _rgb);
    cv::imwrite("test.jpg",_rgb);
    cout << filename_flag << 1  << endl;
    cv::imwrite("img/depth_"+to_string(filename_flag)+".jpg", _depth);
    cout << filename_flag << 2 << endl;
  }
}


/////////////////////////////////////////////////////////////////////////////
// Called every frame gets the data from streams and displays them using OpenCV.
/////////////////////////////////////////////////////////////////////////////
bool display_next_frame( )
{
	// Get current frames intrinsic data.
	_depth_intrin 	= _rs_camera->get_stream_intrinsics( rs::stream::depth );
	_color_intrin 	= _rs_camera->get_stream_intrinsics( rs::stream::color );
	_depth_align_intrin = _rs_camera->get_stream_intrinsics( rs::stream::depth_aligned_to_color );
	_color_align_intrin 	= _rs_camera->get_stream_intrinsics( rs::stream::color_aligned_to_depth );
	// Create depth image
	cv::Mat depth16( _depth_intrin.height,
					 _depth_intrin.width,
					 CV_16U,
					 (uchar *)_rs_camera->get_frame_data( rs::stream::depth ) );

	// Create color image
	cv::Mat rgb( _color_intrin.height,
				 _color_intrin.width,
				 CV_8UC3,
				 (uchar *)_rs_camera->get_frame_data( rs::stream::color ) );
	
	cv::Mat depth16_a(_depth_align_intrin.height,
			  _depth_align_intrin.width,
			  CV_16U,
			  (uchar *)_rs_camera->get_frame_data( rs::stream::depth_aligned_to_color) );
	cv::Mat rgb_a( _color_align_intrin.height,
			  _color_align_intrin.width,
			  CV_8UC3,
			  (uchar *)_rs_camera->get_frame_data( rs::stream::color_aligned_to_depth ) );

	// < 800
	cv::Mat depth8u = depth16;
	depth8u.convertTo( depth8u, CV_8UC1, 255.0/1000 );
	imshow( WINDOW_DEPTH, depth8u );
	cvWaitKey( 1 );
	
	cv::cvtColor( rgb_a, rgb_a, cv::COLOR_BGR2RGB );
	imshow( WINDOW_RGB_ALIGN, rgb_a );
	cvWaitKey( 1 );
	
	cv::Mat depth8u_align = depth16_a;
	depth8u_align.convertTo( depth8u_align, CV_8UC1, 255.0/1000);
	imshow( WINDOW_DEPTH_ALIGN, depth8u_align );
	cvWaitKey( 1 );
	
	cv::cvtColor( rgb, rgb, cv::COLOR_BGR2RGB );
	imshow( WINDOW_RGB, rgb );
	cvWaitKey( 1 );
	
	ifSaveImage(rgb,depth8u_align);
	

	return true;
}



/////////////////////////////////////////////////////////////////////////////
// Main function
/////////////////////////////////////////////////////////////////////////////
int main( ) try
{
	rs::log_to_console( rs::log_severity::warn );

	if( !initialize_streaming( ) )
	{
		std::cout << "Unable to locate a camera" << std::endl;
		rs::log_to_console( rs::log_severity::fatal );
		return EXIT_FAILURE;
	}

	setup_windows( );

	// Loop until someone left clicks on either of the images in either window.
	while( _loop )
	{
		if( _rs_camera->is_streaming( ) )
			_rs_camera->wait_for_frames( );

		display_next_frame( );
	}

	_rs_camera->stop( );
	cv::destroyAllWindows( );

	return EXIT_SUCCESS;
}
catch( const rs::error & e )
{
	std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch( const std::exception & e )
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}