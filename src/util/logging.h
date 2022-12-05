#ifndef LOGGING_H
#define LOGGING_H

#include <boost/log/trivial.hpp>
#include "statusCode.h"

#define	LOG_TRIVIAL(lvl) \
	BOOST_LOG_TRIVIAL(lvl) << "[" << __FILE__ << ":" << __LINE__ << "] "

void initLogger(const boost::log::trivial::severity_level logLevel);

#endif//LOGGING_H
