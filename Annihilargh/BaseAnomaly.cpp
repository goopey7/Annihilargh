#include "BaseAnomaly.h"
#include <sstream>

BaseAnomaly::BaseAnomaly(int line, const char* file) noexcept
	: line(line), file(file)
{
}

const char* BaseAnomaly::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str(); // whatBuffer persists after function, oss does not.
	return whatBuffer.c_str();
}

const char* BaseAnomaly::GetType() const noexcept
{
	return "Base Anomaly";
}

int BaseAnomaly::GetLine() const noexcept
{
	return line;
}

const std::string& BaseAnomaly::GetFile() const noexcept
{
	return file;
}

std::string BaseAnomaly::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "FILE: " << file << std::endl <<
		"LINE: " << line << std::endl;
	return oss.str();
}
