#pragma once

#include <fstream>
#include <plog/Appenders/RollingFileAppender.h>
#include "db_csvformatter.hpp"


#define LOG_TITLES_3_STICKERS(D2units)"Frame_index,Color idx,Depth idx,Color timestamp,Depth timestamp,System color timestamp,System depth timestamp\
,System timestamp,left (x y z) cm,right (x y z) cm,mid3  (x y z) cm,left (x y)" + D2units + ",right (x y) " + D2units + ",mid3 (x y) " + D2units + \
",left - mid3 2D distance " + D2units + ",left - right 2D distance " + D2units + ",right - mid3 2D distance " + D2units + "left - mid3 3D distance (cm)\
,left - right 3D distance (cm),right - mid3 3D distance (cm),2D average distance,3D average distance,FPS,realSamplesNum,frequency,BPM\n"

#define LOG_TITLES_4_STICKERS(D2units)"Frame_index,Color idx,Depth idx,Color timestamp,Depth timestamp,System color timestamp,System depth timestamp\
,System timestamp,left (x y z) cm,right (x y z) cm,mid2  (x y z) cm,mid3  (x y z) cm,left (x y)" + D2units + ",right (x y) " + D2units + ",mid2 (x y) " + D2units + \
",mid3 (x y) " + D2units + ",left - mid2 2D distance " + D2units + ",left - mid3 2D distance " + D2units + ",left - right 2D distance " + D2units + \
",right - mid2 2D distance " + D2units + ",right - mid3 2D distance " + D2units + ",mid2 - mid3 2D distance " + D2units + ",left - mid2 3D distance (cm)\
,left - mid3 3D distance (cm),left - right 3D distance (cm),right - mid2 3D distance (cm),right - mid3 3D distance (cm),mid2 - mid3 3D distance (cm)\
,2D average distance,3D average distance,FPS,realSamplesNum,frequency,BPM\n"

#define LOG_TITLES_5_STICKERS(D2units) "Frame_index,Color idx,Depth idx,Color timestamp,Depth timestamp,System color timestamp,System depth timestamp\
,System timestamp,left (x y z) cm,right (x y z) cm,mid1(x y z) cm,mid2  (x y z) cm,mid3  (x y z) cm,left (x y)" + D2units + ",right (x y) " + D2units + \
",mid1 (x y) " + D2units + ",mid2 (x y) " + D2units + ",mid3 (x y) " + D2units + ",left - mid1 2D distance " + D2units + ",left - mid2 2D distance " + D2units + \
",left - mid3 2D distance " + D2units + ",left - right 2D distance " + D2units + ",right - mid1 2D distance " + D2units + ",right - mid2 2D distance " + \
D2units + ",right - mid3 2D distance " + D2units + ",mid1 - mid2 2D distance " + D2units + ",mid1 - mid3 2D distance " + D2units + ",mid2 - mid3 2D distance " + \
 D2units + ",left - mid1 3D distance (cm),left - mid2 3D distance (cm),left - mid3 3D distance (cm),left - right 3D distance (cm),right - mid1 3D distance (cm),\
right - mid2 3D distance (cm),right - mid3 3D distance (cm),mid1 - mid2 3D distance (cm),mid1 - mid3 3D distance (cm),mid2 - mid3 3D distance (cm),2D average \
distance,3D average distance,FPS,realSamplesNum,frequency,BPM\n"

class DeepBreathLog {
public:
	static void init(bool file_mode);
	static void stop();
protected:

	static void set_csv_headers();

	static plog::RollingFileAppender<DeepBreathCSVFormatter> csv_logger;
};