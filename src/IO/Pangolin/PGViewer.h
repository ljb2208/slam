#pragma once
#include <pangolin/pangolin.h>
#include "util/MinimalImage.h"
#include "boost/thread.hpp"
#include <map>
#include <deque>

namespace slam
{

namespace IOWrap
{

class PGViewer
{
public:
    PGViewer(int w, int h, bool startRunThread=true);

    void run();
	void close();
    void join();

    void reset();
    void reset_internal();


private:    
    int w, h;
    bool running;
    bool needReset;

    boost::thread runThread;

    // images rendering
	boost::mutex openImagesMutex;
	MinimalImageB3* internalVideoImg;
	MinimalImageB3* internalKFImg;
	MinimalImageB3* internalResImg;
	MinimalImageB3* internalVideoImg_Right;
	bool videoImgChanged, kfImgChanged, resImgChanged;


	// 3D model rendering
	boost::mutex model3DMutex;
};
}
}