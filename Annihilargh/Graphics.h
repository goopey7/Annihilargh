#pragma once
#include "WindowsWithoutTheCrap.h"
#include <d3d11.h>
#include <wrl.h>
#include "BaseAnomaly.h"

class Graphics
{
public:
	class Anomaly : public BaseAnomaly
	{
		using BaseAnomaly::BaseAnomaly;
	};
	class AnomalyHresult : public Anomaly
	{
	public:
		AnomalyHresult(int line,const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
	class DeviceRemovedAnomaly : public AnomalyHresult
	{
		using AnomalyHresult::AnomalyHresult;
	public:
		const char* GetType() const noexcept override;
	};
	Graphics(HWND hWnd);
	~Graphics()=default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();

	void ClearBuffer(float r, float g, float b) noexcept;

	void DrawTestTriangle();

private:
	// smart ptrs that already know the COM interface. Good stuff
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView = nullptr;
};
