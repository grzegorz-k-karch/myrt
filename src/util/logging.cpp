#include <iostream>
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "logging.h"

namespace logging = boost::log;

StatusCode initLogger(const boost::log::trivial::severity_level logLevel)
{
  StatusCode status = StatusCode::NoError
  try {
    logging::register_simple_formatter_factory<
      logging::trivial::severity_level, char>("Severity");

    // std::string msgFormatCout = "[%TimeStamp%] [%LineID%] [%Severity%] %Message%";
    std::string msgFormatCout = "[%TimeStamp%] [%Severity%] %Message%";  
    std::string msgFormatFile = msgFormatCout;

    logging::add_file_log
      (
       boost::log::keywords::file_name = "sample.log",
       boost::log::keywords::format = msgFormatCout
       );

    logging::add_console_log
      (
       std::cout,
       boost::log::keywords::format = msgFormatFile
       );

    logging::core::get()->set_filter
      (
       logging::trivial::severity >= logLevel
       );

    logging::add_common_attributes();
  }
  catch (const boost::exception& ex) {
    std::cerr << boost::diagnostic_information(ex) << std::endl;
    status = StatusCode::LoggingError;
  }
  return status;
}
