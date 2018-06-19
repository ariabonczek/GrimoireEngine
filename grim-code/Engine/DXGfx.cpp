#include "Precompiled.h"

#include <Engine/Gfx.h>
#include <Engine/DXGfx.h>
#include <Engine/EngineGlobals.h>
#include <Engine/DrawElement.h>
#include <Engine/GraphicsState.h>
#include <Engine/Surface.h>
#include <Engine/grimInput.h>

#include <Engine/d3dx12.h>
#include <string>
#include <d3dcompiler.h>

namespace plat {

	static HWND s_window;
	static IDXGISwapChain3* s_swapChain;
	static ID3D12Device* s_device;
	static ID3D12CommandQueue* s_commandQueue[gfx::kNumCommandQueues];
	
	ID3D12Device* GetDevice()
	{
		return s_device;
	}

	ID3D12CommandQueue* GetCommandQueue(int index)
	{
		GRIM_ASSERT(index <= gfx::kNumCommandQueues, "Invalid command queue index requested");
		return s_commandQueue[index];
	}

	IDXGISwapChain3* GetSwapChain()
	{
		return s_swapChain;
	}

	static LRESULT HandleMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_CLOSE:
			case WM_QUIT:
				gfx::TriggerClose();
				return 0;
			case WM_SIZE:
				gfx::ResizeFramebuffer(LOWORD(lParam), HIWORD(lParam));
				return 0;
			case WM_ENTERSIZEMOVE:
				return 0;
			case WM_EXITSIZEMOVE:
				return 0;
			case WM_DESTROY:
				gfx::TriggerClose();
				return 0;
			case WM_LBUTTONDOWN:
				return 0;
			case WM_LBUTTONUP:
				return 0;
			case WM_KEYDOWN:
				return 0;
			case WM_MENUCHAR:
				return MAKELRESULT(0, MNC_CLOSE);
			case WM_GETMINMAXINFO:
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
				return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return HandleMessages(hwnd, msg, wParam, lParam);
	}

	static void InitWindow()
	{
		const HINSTANCE hInstance = GetModuleHandle(NULL);

		if (g_engineGlobals.bootingFromEditor)
		{
			s_window = (HWND)g_engineGlobals.windowHandle;
			return;
		}

		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = "Grimoire";
		int ret = RegisterClassEx(&windowClass);

		GRIM_ASSERT(ret, "Failed to register window class");

		RECT windowRect = { 0, 0, static_cast<LONG>(g_engineGlobals.windowWidth), static_cast<LONG>(g_engineGlobals.windowHeight) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and store a handle to it.
		s_window = CreateWindow(
			windowClass.lpszClassName,
			g_engineGlobals.windowTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

		GRIM_ASSERT(s_window, "Failed to create window");

		const int nCmdShow = SW_SHOWDEFAULT;

		ShowWindow(s_window, nCmdShow);

	}

	static void InitDebugLayer()
	{
		ID3D12Debug* debugLayer;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
		{
			debugLayer->EnableDebugLayer();
		}
	}

	static void InitDeviceAndCommandQueues(IDXGIFactory4* factory)
	{
		// TODO: Support warp adapter

		IDXGIAdapter1* chosenAdapter = nullptr;
		IDXGIAdapter1* currentAdapter = nullptr;

		for (int i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &currentAdapter); ++i)
		{
			DXGI_ADAPTER_DESC1 currentDesc;
			currentAdapter->GetDesc1(&currentDesc);

			if (currentDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			if (chosenAdapter != nullptr)
			{
				DXGI_ADAPTER_DESC1 chosenDesc;
				chosenAdapter->GetDesc1(&chosenDesc);

				if (chosenDesc.DedicatedVideoMemory > currentDesc.DedicatedVideoMemory)
					continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(currentAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				chosenAdapter = currentAdapter;
		}

		HRESULT hr = D3D12CreateDevice(chosenAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_device));
		GRIM_ASSERT(!hr, " Failed to create device");
		s_device->SetName(L"DX12 Device");

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hr = s_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_commandQueue[gfx::kGfxQueue]));
		GRIM_ASSERT(!hr, "Failed to create command queue");
		s_commandQueue[gfx::kGfxQueue]->SetName(L"Gfx Queue");

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr = s_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_commandQueue[gfx::kCompute0]));
		GRIM_ASSERT(!hr, "Failed to create command queue");
		s_commandQueue[gfx::kCompute0]->SetName(L"Compute Queue 0");

		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		hr = s_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_commandQueue[gfx::kCompute1]));
		GRIM_ASSERT(!hr, "Failed to create command queue");
		s_commandQueue[gfx::kCompute1]->SetName(L"Compute Queue 1");
	}

	static void InitSwapChain(IDXGIFactory4* factory)
	{
		// Create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = g_engineGlobals.numDisplayBuffers;
		swapChainDesc.Width = g_engineGlobals.frameBufferWidth;
		swapChainDesc.Height = g_engineGlobals.frameBufferHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		IDXGISwapChain1* temp;
		HRESULT hr = factory->CreateSwapChainForHwnd(plat::GetCommandQueue(gfx::kGfxQueue), s_window, &swapChainDesc, nullptr, nullptr, &temp);
		GRIM_ASSERT(!hr, "Failed to create swap chain");
		factory->MakeWindowAssociation(s_window, DXGI_MWA_VALID);
		GRIM_ASSERT(!hr, "Failed to associate swap chain with window");

		s_swapChain = static_cast<IDXGISwapChain3*>(temp);
	}

	void InitGfx()
	{
		InitWindow();

		UINT factoryFlags = 0;

#ifdef _DEBUG
		InitDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		IDXGIFactory4* factory;
		HRESULT hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));
		GRIM_ASSERT(!hr, "Failed to create factory");

		InitDeviceAndCommandQueues(factory);
		InitSwapChain(factory);

		factory->Release();
	}

	void ProcessGfx()
	{
		// Message loop
		MSG msg = {};
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			grimInput::HandleMessage(msg);
		}
	}

	void ShutdownGfx()
	{
		for (int i = 0; i < gfx::kNumCommandQueues; ++i)
		{
			s_commandQueue[i]->Release();
		}

		s_swapChain->Release();

#ifdef _DEBUG
		ID3D12DebugDevice* debugDevice;
		HRESULT hr = s_device->QueryInterface(IID_PPV_ARGS(&debugDevice));
		
		if (!hr)
		{
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
		}
#endif

		s_device->Release();
	}

	void Flip()
	{
		s_swapChain->Present(1, 0);
	}
	
	int GetCurrentBackbufferIndex()
	{
		return s_swapChain->GetCurrentBackBufferIndex();
	}

	void SetWindowTitle(const char* text)
	{
		SetWindowText(s_window, text);
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC StateToPipelineDesc(const gfx::GraphicsState& graphicsState, const gfx::ResourceState& resourceState, const gfx::Surface* renderTargets[8], const gfx::Surface* depthRenderTarget)
	{
		// Rasterizer State
		D3D12_RASTERIZER_DESC rd = {};
		{
			rd.FillMode = (D3D12_FILL_MODE)graphicsState.rasterizerState.fillMode;
			rd.CullMode = (D3D12_CULL_MODE)graphicsState.rasterizerState.cullMode;
			rd.FrontCounterClockwise = (BOOL)graphicsState.rasterizerState.frontCounterClockwise;
		}

		// BlendState
		D3D12_BLEND_DESC bd = {};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			bd.RenderTarget[i].BlendEnable = (BOOL)graphicsState.blendState[i].blendEnable;
			bd.RenderTarget[i].SrcBlend = (D3D12_BLEND)graphicsState.blendState[i].srcColor;
			bd.RenderTarget[i].DestBlend = (D3D12_BLEND)graphicsState.blendState[i].destColor;
			bd.RenderTarget[i].BlendOp = (D3D12_BLEND_OP)graphicsState.blendState[i].colorFunc;
			bd.RenderTarget[i].SrcBlendAlpha = (D3D12_BLEND)graphicsState.blendState[i].srcAlpha;
			bd.RenderTarget[i].DestBlendAlpha = (D3D12_BLEND)graphicsState.blendState[i].destAlpha;
			bd.RenderTarget[i].BlendOpAlpha = (D3D12_BLEND_OP)graphicsState.blendState[i].alphaFunc;
			bd.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		// DepthStencilState
		D3D12_DEPTH_STENCIL_DESC dsd = {};
		{
			dsd.DepthEnable = graphicsState.depthStencilState.testEnabled;
			dsd.DepthWriteMask = graphicsState.depthStencilState.writeEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			dsd.DepthFunc = (D3D12_COMPARISON_FUNC)graphicsState.depthStencilState.depthFunc;
		}

		// MultisampleState
		DXGI_SAMPLE_DESC msd = {};
		{
			msd.Count = graphicsState.multisampleState.count;
			msd.Quality = graphicsState.multisampleState.quality;
		}

		// Create the pipeline state, which includes compiling and loading shaders.
		{
			const plat::DXShader& shader = resourceState.shader->platformShader;

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.pRootSignature = shader.rootSignature;
			desc.InputLayout = shader.inputLayout;
			desc.VS = CD3DX12_SHADER_BYTECODE(shader.vertexShader);
			desc.PS = CD3DX12_SHADER_BYTECODE(shader.pixelShader);
			desc.RasterizerState = rd;
			desc.BlendState = bd;
			desc.DepthStencilState = dsd;
			desc.SampleDesc = msd;
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			int count = 0;
			for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			{
				if (renderTargets[i])
				{
					desc.RTVFormats[i] = (DXGI_FORMAT) renderTargets[i]->m_definition.format;
					++count;
				}
			}
			desc.NumRenderTargets = count;

			if (depthRenderTarget)
				desc.DSVFormat = (DXGI_FORMAT) depthRenderTarget->m_definition.format;

			// Note: this is valid for compute shaders!
			GRIM_ASSERT(count, "Creating a graphics pipeline state with no render targets");

			return desc;
		}
	}
}