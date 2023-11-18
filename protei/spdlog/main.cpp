#include "spdlog/spdlog.h"

int main() {
	//by default there are no debug and trace level in log
	//to add all possible logging level, use:
	spdlog::default_logger()->set_level(spdlog::level::trace);

	spdlog::trace("TRACE"); //LogLevel:: TRACE
	spdlog::debug("DEBUG"); //LogLevel:: DEBUG
	spdlog::info("INFO"); //LogLevel:: INFO
	spdlog::warn("WARN"); //LogLevel:: WARN
	spdlog::error("ERROR"); //LogLevel:: ERROR
	spdlog::critical("CRITICAL"); //LogLevel:: CRITICAL
	return 0;
}
