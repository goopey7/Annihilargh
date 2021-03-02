#include "Image.h"

#include <algorithm>
#include <cassert>

#include <sstream>

#include <Unknwn.h>
#include "WindowsWithoutTheCrap.h"
// gdiplus needs min and max but we disabled that in the windows header. So we will provide the std::min and std::max
// to the gdiplus namespace.
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
namespace gdi = Gdiplus;
#pragma comment( lib,"gdiplus.lib" )

Image::Anomaly::Anomaly(int line, const char* file, std::string desc) noexcept
: BaseAnomaly(line,file),errorMsg(std::move(desc))
{}

const char* Image::Anomaly::what() const noexcept
{
	std::stringstream ss;
	ss << BaseAnomaly::what() << std::endl
	<< "Message: " << GetErrorMessage();
	whatBuffer = ss.str();
	return whatBuffer.c_str();
}

const char* Image::Anomaly::GetType() const noexcept
{
	return "Image Anomaly";
}

const std::string& Image::Anomaly::GetErrorMessage() const noexcept
{
	return errorMsg;
}

Image::Image(unsigned int width, unsigned int height) noexcept
: width(width),height(height),pBuffer( std::make_unique<Colour[]>(width*height))
{}

Image::Image(Image&& src) noexcept
: width(src.width),height(src.height),pBuffer( std::move(src.pBuffer))
{
}

Image& Image::operator=(Image&& pp) noexcept
{
	width = pp.width;
	height = pp.height;
	pBuffer = std::move(pp.pBuffer);
	pp.pBuffer = nullptr;
	return *this;
}

void Image::Clear(Colour fillVal) noexcept
{
	memset(pBuffer.get(),fillVal.dword,width*height*sizeof(Colour));
}

void Image::PutPixel(unsigned x, unsigned y, Colour c) noexcept
{
	assert(x>=0);
	assert(y>=0);
	assert(x<width);
	assert(y<height);
	// this is the formula for accessing a 2D array in 1D
	pBuffer[y*width+x] = c;
}

Image::Colour Image::GetPixel(unsigned x, unsigned y) const noexcept
{
	assert(x>=0);
	assert(y>=0);
	assert(x<width);
	assert(y<height);
	return pBuffer[y*width+x];
}

unsigned Image::GetWidth() const noexcept
{
	return width;
}

unsigned Image::GetHeight() const noexcept
{
	return height;
}

Image::Colour* Image::GetBufferPtr() noexcept
{
	return pBuffer.get();
}

const Image::Colour* Image::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

Image Image::FromFile(const std::string& fileName)
{
	unsigned int width=0,height=0;
	std::unique_ptr<Colour[]> pBuffer;

	// provide another level of scope
	{
		// convert filename to wstring for gdiplus
		wchar_t wFileName[512];
		mbstowcs_s(nullptr,wFileName,fileName.c_str(),_TRUNCATE);

		gdi::Bitmap bitmap(wFileName);
		if(bitmap.GetLastStatus()!=gdi::Status::Ok)
		{
			std::stringstream ss;
			ss << "Failed to load image: " << fileName;
			throw Anomaly(__LINE__,__FILE__,ss.str());
		}

		width = bitmap.GetWidth();
		height=bitmap.GetHeight();
		pBuffer = std::make_unique<Colour[]>(width*height);

		// column major order
		for(unsigned int y=0;y<height;y++)
		{
			for(unsigned int x =0;x<width;x++)
			{
				gdi::Color c;
				bitmap.GetPixel(x,y,&c);
				pBuffer[y*width+x] = c.GetValue();
			}
		}
		return Image(width,height,std::move(pBuffer));
	}
}

void Image::Copy(const Image& src) noexcept
{
	assert(width==src.width);
	assert(height==src.height);
	memcpy(pBuffer.get(),src.pBuffer.get(),width*height*sizeof(Colour));
}

Image::Image(unsigned width, unsigned height, std::unique_ptr<Colour[]> pBufferParam) noexcept
: width(width),height(height),pBuffer(std::move(pBufferParam))
{}






