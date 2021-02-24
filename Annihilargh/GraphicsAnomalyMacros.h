// just give our throw macros their own header files instead of copy/pasting them
#ifndef GRAPHICSANOMALYMACROS_H
#define GRAPHICSANOMALYMACROS_H


// checks the return value of an expression that returns an hresult to see if it is a failure code
// if it is we throw a gfx anomaly with the hresult
// expects the hr variable to be in the local scope
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr=(hrcall)))throw Graphics::AnomalyHresult(__LINE__,__FILE__,hr)

// creates a device removed anomaly
#define GFX_DEVICE_REMOVED_ANOMALY(hr) Graphics::DeviceRemovedAnomaly(__LINE__,__FILE__,(hr))

#endif // GRAPHICSANOMALYMACROS_H
