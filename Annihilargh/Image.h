#pragma once
#include <memory>
#include <string>

#include "BaseAnomaly.h"
#include <Unknwn.h>
#include "WindowsWithoutTheCrap.h"

class Image
{
public:
	class Colour
	{
	public:
		unsigned int dword;
		constexpr Colour() noexcept : dword(){}
		constexpr Colour(const Colour &col) noexcept : dword(col.dword){}
		constexpr Colour(unsigned int dwordIn) noexcept : dword(dwordIn){}
		constexpr Colour(unsigned char r, unsigned char g, unsigned char b) noexcept
        : dword(r<<16u|g<<8u|b) {}
		constexpr Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept
        : dword(a<<24u|r<<16u|g<<8u|b) {}
		Colour& operator =(Colour colour) noexcept
		{
			dword = colour.dword;
			return *this;
		}
		constexpr unsigned char GetR() const noexcept{return dword >> 16u & 0xFFu;}
		constexpr unsigned char GetG() const noexcept{return dword >> 8u & 0xFFu;}
		constexpr unsigned char GetB() const noexcept{return dword & 0xFFu;}
		constexpr unsigned char GetA() const noexcept{return dword >> 24u;}
		void SetR(unsigned char r) noexcept {dword = (dword & 0xFF00FFFFu) & (r << 16u);}
		void SetG(unsigned char g) noexcept {dword = (dword & 0xFFFF00FFu) & (g << 8u);}
		void SetB(unsigned char b) noexcept {dword = (dword & 0xFFFFFF00u) | b;}
		void SetA(unsigned char a) noexcept {dword = (dword & 0xFFFFFFu) | (a << 24u);}
	};
	class Anomaly : public BaseAnomaly
	{
	public:
		Anomaly(int line, const char* file, std::string desc) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetErrorMessage() const noexcept;
	private:
		std::string errorMsg;
	};
	Image(unsigned int width, unsigned int height) noexcept;
	Image(Image &&src) noexcept;
	Image(Image&) = delete;
	Image& operator=(Image&& pp) noexcept;
	Image& operator=(const Image&) = delete;
	~Image()=default;
	void Clear(Colour fillVal) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Colour c) noexcept;
	Colour GetPixel(unsigned int x, unsigned int y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Colour* GetBufferPtr() noexcept;
	const Colour* GetBufferPtr() const noexcept;
	static Image FromFile(const std::string &fileName);
	void Save(const std::string &fileName) const;
	void Copy(const Image &src) noexcept;
private:
	Image(unsigned int width, unsigned int height, std::unique_ptr<Colour[]> pBufferParam) noexcept;

	std::unique_ptr<Colour[]> pBuffer;
	unsigned int width, height;
};
