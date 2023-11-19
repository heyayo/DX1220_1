#ifndef DX1220_LOGGING_HPP
#define DX1220_LOGGING_HPP

#include <iostream>

#define LOGINFO(msg) std::cout << "[INFO] " << msg << std::endl
#define LOGWARN(msg) std::cout << "[WARN] " << msg << std::endl
#define LOGERROR(msg) std::cerr << "[ERROR " << msg << std::endl

#endif
