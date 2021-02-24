// just give our throw macros their own header files instead of copy/pasting them
#ifndef WINDOWANOMALYMACROS_H
#define WINDOWANOMALYMACROS_H
#pragma once

// cheeky macro for getting the line and file from the hresult
#define WND_ANOMALY(hr) Window::Anomaly(__LINE__,__FILE__,hr)

// some windows functions don't return an hresult, so here's another version that gets the last error
#define WND_ANOMALY_LAST_ERROR() Window::Anomaly(__LINE__,__FILE__,GetLastError())

#define WND_NOGFX_ANOMALY() Window::NoGraphicsAnomaly(__LINE__,__FILE__)
#endif // WINDOWANOMALYMACROS_H
