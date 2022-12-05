#ifndef STATUS_CODES_H
#define STATUS_CODES_H

enum class StatusCode {
  NoError, FileError, CmdLineError, SceneError, UnknownError, CudaError, OptixError, LoggingError
    };

#endif//STATUS_CODES_H
