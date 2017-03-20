#include <d3d11.h>

#include <Windows.h>
#include <tchar.h>

#include "Shader\ps.h"
#include "Shader\vs.h"

#pragma once
#pragma comment(lib, "d3d11.lib")

#define SAFE_RELEASE(p) {if(p) {(p)->Release(); (p) = NULL;}}

// ウィンドウのコールバック関数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
	TCHAR szWindowsClass[] = "3DDISPPG";
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowsClass;
	RegisterClass(&wcex);

	// ウィンドウをクリエイト
	HWND hWnd;
	hWnd = CreateWindowA(szWindowsClass,
		"3D Disp Pg",
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | TLS_MINIMUM_AVAILABLE,
		0,
		0,
		1280,
		720,
		NULL,
		NULL,
		hInst,
		NULL);

	//ShowWindow(hWnd, nCmdShow);

	// DirectX11の初期化
	// デバイスの生成
	HRESULT hr;
	ID3D11Device* hpDevice = NULL;
	ID3D11DeviceContext* hpDeviceContext = NULL;
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&hpDevice,
		NULL,
		&hpDeviceContext);

	if (FAILED(hr)) {
		MessageBox(hWnd, _T("D3D11CreateDevice"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// インターフェース取得
	IDXGIDevice1* hpDXGI1 = NULL;
	if (FAILED(hpDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI1))) {
		MessageBox(hWnd, _T("QueryDavice"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// アダプター取得
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI1->GetAdapter(&hpAdapter))) {
		MessageBox(hWnd, _T("GetAdapter"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// ファクトリー取得
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL) {
		MessageBox(hWnd, _T("GetParent"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// ALT+ENTERでフルスクリーンを許可する
	if (FAILED(hpDXGIFactory->MakeWindowAssociation(hWnd, 0))) {
		MessageBox(hWnd, _T("MakeWindowAssociation"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// スワップチェイン作成
	IDXGISwapChain* hpDXGISwapChain = NULL;
	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = 1280;
	hDXGISwapChainDesc.BufferDesc.Height = 720;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = hWnd;
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(hpDevice, &hDXGISwapChainDesc, &hpDXGISwapChain))) {
		MessageBox(hWnd, _T("CreateSwapChain"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// スワップチェインのバックバッファを取得
	ID3D11Texture2D* hpBackFuffer = NULL;
	if (FAILED(hpDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&hpBackFuffer))){
		MessageBox(hWnd, _T("SwapChain GetBuffer"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// バックバッファから描画ターゲットを生成
	ID3D11RenderTargetView* hpRenderTargetView = NULL;
	if (FAILED(hpDevice->CreateRenderTargetView(hpBackFuffer, NULL, &hpRenderTargetView))) {
		MessageBox(hWnd, _T("CreateRenderTargetView"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// 描画ターゲットをコンテキストに設定
	hpDeviceContext->OMSetRenderTargets(1, &hpRenderTargetView, NULL);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	hpDeviceContext->RSSetViewports(1, &vp);

	// 三角形の表示はここから
	struct Vertex3D
	{
		float pos[3];
		float col[4];
	};

	const int TYOUTEN = 3;

	// 頂点データ
	Vertex3D hVectorData[TYOUTEN] = {
		{ { +0.0f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
	};

	// 頂点レイアウト
	// 5番目のパラメータは先頭からのバイト数なので、COLORにはPOSITIONのfloat型＊3を記述
	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4*3, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// 頂点バッファ作成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(Vertex3D) * TYOUTEN;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = hVectorData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* hpBuffer;
	if (FAILED(hpDevice->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
		MessageBox(hWnd, _T("CreateBuffer"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// 頂点バッファをコンテキストに設定
	UINT hStrides = sizeof(Vertex3D);
	UINT hOffsets = 0;
	hpDeviceContext->IASetVertexBuffers(0, 1, &hpBuffer, &hStrides, &hOffsets);

	// プリミティブ(ポリゴンの形状)をコンテキストに設定
	hpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点レイアウト作成
	ID3D11InputLayout* hpInputLayout = NULL;
	if (FAILED(hpDevice->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), &g_vs_main, sizeof(g_vs_main), &hpInputLayout))) {
		MessageBox(hWnd, _T("CreateInputLayout"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// 頂点レイアウトをコンテキストに作成
	hpDeviceContext->IASetInputLayout(hpInputLayout);

	// 頂点シェーダ生成
	ID3D11VertexShader* hpVertexShader;
	if (FAILED(hpDevice->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, &hpVertexShader))) {
		MessageBox(hWnd, _T("CreateVertexShader"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// 頂点シェーダをコンテキストに設定
	hpDeviceContext->VSSetShader(hpVertexShader, NULL, 0);

	// ピクセルシェーダを生成
	ID3D11PixelShader* hpPixelShader;
	if (FAILED(hpDevice->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, &hpPixelShader))) {
		MessageBox(hWnd, _T("CreatePixelShader"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// ピクセルシェーダをコンテキストに設定
	hpDeviceContext->PSSetShader(hpPixelShader, NULL, 0);

	// メインループ
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto END;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}

		// 背景描画
		float ClearColor[]{ 0.0f, 0.0f, 1.0f, 1.0f };
		hpDeviceContext->ClearRenderTargetView(hpRenderTargetView, ClearColor);

		// 描画
		hpDeviceContext->Draw(TYOUTEN, 0);

		// バックバッファをスワップ
		hpDXGISwapChain->Present(0, 0);
	}
END:
	// リリース
	SAFE_RELEASE(hpPixelShader);
	SAFE_RELEASE(hpVertexShader);
	SAFE_RELEASE(hpInputLayout);

	// ポインタで生成したものをリリース
	SAFE_RELEASE(hpRenderTargetView);
	SAFE_RELEASE(hpBackFuffer);
	SAFE_RELEASE(hpDXGISwapChain);
	SAFE_RELEASE(hpDXGIFactory);
	SAFE_RELEASE(hpAdapter);
	SAFE_RELEASE(hpDXGI1);
	SAFE_RELEASE(hpDeviceContext);
	SAFE_RELEASE(hpDevice);

	return 0;
}
