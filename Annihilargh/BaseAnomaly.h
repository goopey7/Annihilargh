#pragma once
#include <exception>
#include <string>

class BaseAnomaly : public std::exception
{
public:
	BaseAnomaly(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int line;
	std::string file;
protected: // can be accessed by any children
	mutable std::string whatBuffer; // mutable allows us to change this in a const function
};

