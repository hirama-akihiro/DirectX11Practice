#include <d3d11.h>

#include <Windows.h>
#include <tchar.h>

#pragma once
#pragma comment(lib, "d3d11.lib")

#define SAFE_RELEASE(p) {if(p) {(p)->Release(); (p) = NULL;}}

// �E�B���h�E�̃R�[���o�b�N�֐�
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

	// �E�B���h�E���N���G�C�g
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

	// DirectX11�̏�����
	// �f�o�C�X�̐���
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

	// �C���^�[�t�F�[�X�擾
	IDXGIDevice1* hpDXGI1 = NULL;
	if (FAILED(hpDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI1))) {
		MessageBox(hWnd, _T("QueryDavice"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// �A�_�v�^�[�擾
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI1->GetAdapter(&hpAdapter))) {
		MessageBox(hWnd, _T("GetAdapter"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// �t�@�N�g���[�擾
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL) {
		MessageBox(hWnd, _T("GetParent"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// ALT+ENTER�Ńt���X�N���[����������
	if (FAILED(hpDXGIFactory->MakeWindowAssociation(hWnd, 0))) {
		MessageBox(hWnd, _T("MakeWindowAssociation"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// �X���b�v�`�F�C���쐬
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

	// �X���b�v�`�F�C���̃o�b�N�o�b�t�@���擾
	ID3D11Texture2D* hpBackFuffer = NULL;
	if (FAILED(hpDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&hpBackFuffer))){
		MessageBox(hWnd, _T("SwapChain GetBuffer"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// �o�b�N�o�b�t�@����`��^�[�Q�b�g�𐶐�
	ID3D11RenderTargetView* hpRenderTargetView = NULL;
	if (FAILED(hpDevice->CreateRenderTargetView(hpBackFuffer, NULL, &hpRenderTargetView))) {
		MessageBox(hWnd, _T("CreateRenderTargetView"), _T("Err"), MB_ICONSTOP);
		goto END;
	}

	// �`��^�[�Q�b�g���R���e�L�X�g�ɐݒ�
	hpDeviceContext->OMSetRenderTargets(1, &hpRenderTargetView, NULL);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	// ���C�����[�v
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto END;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}

		// �Ȃɂ���`��
		float ClearColor[]{ 0.0f, 0.0f, 1.0f, 1.0f };
		hpDeviceContext->ClearRenderTargetView(hpRenderTargetView, ClearColor);
		hpDXGISwapChain->Present(0, 0);
	}
END:
	// �|�C���^�Ő����������̂������[�X
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

/*
#include <d3d11.h>

#include <windows.h>
#include <tchar.h>


//�܂��̓E�B���h�E�̃R�[���o�b�N�֐����L�q
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//�E�B���h�E�N���X��o�^����
	TCHAR szWindowClass[] = "3DDISPPG";
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	//�E�B���h�E���N���G�C�g
	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		"3D Disp Pg",
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,
		0,
		1020,
		1080,
		NULL,
		NULL,
		hInstance,
		NULL);

	//��ŏ������C�Ƃ肠�����E�B���h�E���o���Ȃ��Ƃ킩��h���̂�
	ShowWindow(hWnd, nCmdShow);

	//���C�����[�v
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto End;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}
	}

End:
	return 0;
}
*/