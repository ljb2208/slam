
#include <thread>
#include <locale.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include "IO/Pangolin/PGViewer.h"
#include "util/DataSetReader.h"

int width = 1276;
int height = 376;


std::string vignette = "";
std::string gammaCalib = "";
std::string source = "";
std::string calib = "";

using namespace slam;


int main( int argc, char** argv )
{
    IOWrap::PGViewer* viewer = 0;

    viewer = new IOWrap::PGViewer(width, height, true);

    ImageFolderReader* reader = new ImageFolderReader(source+"/image_0", calib, gammaCalib, vignette);
	//ImageFolderReader* reader_right = new ImageFolderReader(source+"/image_1", calib, gammaCalib, vignette);
	//reader->setGlobalCalibration();
	//reader_right->setGlobalCalibration();

    while (1);;

    return 0;
}