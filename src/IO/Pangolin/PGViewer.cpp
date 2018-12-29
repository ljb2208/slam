#include "PGViewer.h"

#include <iostream>
#include <fstream>
#include <string>

namespace slam
{

namespace IOWrap

{

PGViewer::PGViewer(int w, int h, bool startRunThread)
{
    this->w = w;
    this->h = h;
    running = true;

    {
		boost::unique_lock<boost::mutex> lk(openImagesMutex);
		internalVideoImg = new MinimalImageB3(w,h);
		internalKFImg = new MinimalImageB3(w,h);
		internalResImg = new MinimalImageB3(w,h);
		internalVideoImg_Right = new MinimalImageB3(w,h);
		
		videoImgChanged=kfImgChanged=resImgChanged=true;

		internalVideoImg->setBlack();
		internalVideoImg_Right->setBlack();
		internalKFImg->setBlack();
		internalResImg->setBlack();
	}

	needReset = false;

    if(startRunThread)
        runThread = boost::thread(&PGViewer::run, this);
}

void PGViewer::run()
{
    printf("START PANGOLIN!\n");

	pangolin::CreateWindowAndBind("Main SLAM Window",2*w,2*h);
	const int UI_WIDTH = 180;

	glEnable(GL_DEPTH_TEST);

    // 3D visualization
	pangolin::OpenGlRenderState Visualization3D_camera(
		pangolin::ProjectionMatrix(w,h,400,400,w/2,h/2,0.1,1000),
		pangolin::ModelViewLookAt(-0,-5,-10, 0,0,0, pangolin::AxisNegY)
		);

	pangolin::View& Visualization3D_display = pangolin::CreateDisplay()
		.SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -w/(float)h)
		.SetHandler(new pangolin::Handler3D(Visualization3D_camera));

    	// 4 images
	pangolin::View& d_kfDepth = pangolin::Display("imgKFDepth")
	    .SetAspect(w/(float)h);

	pangolin::View& d_video_Right = pangolin::Display("imgKFDepth_Right")
	    .SetAspect(w/(float)h);

	pangolin::View& d_video = pangolin::Display("imgVideo")
	    .SetAspect(w/(float)h);

	pangolin::View& d_residual = pangolin::Display("imgResidual")
	    .SetAspect(w/(float)h);

	pangolin::GlTexture texKFDepth(w,h,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);
	pangolin::GlTexture texVideo(w,h,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);
	pangolin::GlTexture texVideo_Right(w,h,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);
	pangolin::GlTexture texResidual(w,h,GL_RGB,false,0,GL_RGB,GL_UNSIGNED_BYTE);


    pangolin::CreateDisplay()
		  .SetBounds(0.0, 0.3, pangolin::Attach::Pix(UI_WIDTH), 1.0)
		  .SetLayout(pangolin::LayoutEqual)
		  .AddDisplay(d_kfDepth)
		  .AddDisplay(d_video)
		  .AddDisplay(d_video_Right)
		  .AddDisplay(d_residual);

    // parameter reconfigure gui
	pangolin::CreatePanel("ui").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));

    pangolin::Var<double> settings_trackFps("ui.Track fps",0,0,0,false);
	pangolin::Var<double> settings_mapFps("ui.KF fps",0,0,0,false);


     // Default hooks for exiting (Esc) and fullscreen (tab).
	while( !pangolin::ShouldQuit() && running )
	{
        // Clear entire screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        openImagesMutex.lock();
		if(videoImgChanged) {
		  texVideo.Upload(internalVideoImg->data,GL_BGR,GL_UNSIGNED_BYTE);
		  texVideo_Right.Upload(internalVideoImg_Right->data,GL_BGR,GL_UNSIGNED_BYTE);
		}
		if(kfImgChanged) {
		  texKFDepth.Upload(internalKFImg->data,GL_BGR,GL_UNSIGNED_BYTE);  
		}
		if(resImgChanged) {
            texResidual.Upload(internalResImg->data,GL_BGR,GL_UNSIGNED_BYTE);
        }

		videoImgChanged=kfImgChanged=resImgChanged=false;
		openImagesMutex.unlock();

        // Swap frames and Process Events
		pangolin::FinishFrame();

    }

    exit(1);

}

void PGViewer::reset()
{
    needReset = true;
}

void PGViewer::reset_internal()
{
	openImagesMutex.lock();
	internalVideoImg->setBlack();
	internalVideoImg_Right->setBlack();
	internalKFImg->setBlack();
	internalResImg->setBlack();
	videoImgChanged= kfImgChanged= resImgChanged=true;
	openImagesMutex.unlock();

	needReset = false;
}

void PGViewer::close()
{
    running = false;
}

void PGViewer::join()
{
    runThread.join();
    printf("JOINED Pangolin thread!\n");
}

}
}