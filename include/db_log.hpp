#pragma once

#include <fstream>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Log.h>
#include "db_csvformatter.hpp"

#define DBLOG PLOG_INFO
#define FIXED_PRECISION std::fixed << std::setprecision(2)

class DeepBreathLog {
public:
	static void init(bool file_mode);
	static void stop();
protected:

	static void set_csv_headers();
	static plog::RollingFileAppender<DeepBreathCSVFormatter> csv_logger;
	static bool first_init;
};