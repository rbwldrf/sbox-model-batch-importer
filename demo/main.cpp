// see demo.cpp for OpenFBX related stuff


// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

//MAKE SURE YOUR C++ LANGUAGE STANDARD IS SET TO LATEST
//TODO: implement this setting into the batch file 
//		when i can be arsed to learn shell scripting :þ

#include "../src/ofbx.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <filesystem>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <vector>


// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool initOpenFBX(const char* filepath, HWND hwnd);
void demoGUI();

ofbx::IScene* loadModel(std::string filepath);


// Main code
int main(int, char**)
{
	// Create application window
	// ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = {sizeof(wc),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		L"ImGui Example",
		nullptr};
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName,
		L"Dear ImGui DirectX11 Example",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		1280,
		800,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	  // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;
	// io.ConfigViewportsNoDefaultParent = true;
	// io.ConfigDockingAlwaysTabBar = true;
	// io.ConfigDockingTransparentPayload = true;
	// io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T
	// WORK AS EXPECTED. DON'T USE IN USER APP! io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; //
	// FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
	// ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// std::vector<ofbx::IScene*> scenes{};
	ofbx::IScene* scene{};
	std::string path = "C:\\Projects\\gridlock\\models\\src";
	std::string dest = "C:\\Projects\\gridlock\\models\\";
	std::string pref = "batch_importer_test";
	int i = 0;
	for (const auto& bla : std::filesystem::directory_iterator(path))
	{
		if (bla.path().extension() == ".fbx")
		{
			scene = (loadModel(bla.path().string()));
			// bla.path().filename();
			// std::cout << ((const char*)(bla.path().string().data())) << std::endl;
			std::cout << ((const char*)(scene->getGeometry(0)->name)) << std::endl;

			auto dir = dest;
			auto prefix = dir.append(pref);
			auto suffix = prefix.append(std::to_string(i));
			auto yippee = suffix.append(".vmdl");

			std::cout << yippee << std::endl;

			// create file, overwrite it if it has to
			std::ofstream out(yippee);

			std::ostringstream oss;

			// TODO: automate this for the love of christ
			oss << "<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} "
				   "format:modeldoc29:version{3cec427c-1b0e-4d48-a90a-0436f33a6041} -->\n"
				<< "{\n"
				<< "\trootNode =\n"
				<< "\t{\n"
				<< "\t\t_class = \"RootNode\"\n"
				<< "\t\tchildren =\n"
				<< "\t\t[\n"
				<< "\t\t\t{\n"
				<< "\t\t\t\t_class = \"MaterialGroupList\"\n"
				<< "\t\t\t\tchildren =\n"
				<< "\t\t\t\t[\n"
				<< "\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t_class = \"DefaultMaterialGroup\"\n"
				<< "\t\t\t\t\t\tremaps = \n"
				<< "\t\t\t\t\t\t[\n"
				<< "\t\t\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t\t\tfrom = \"buildings.vmat\"\n"
				<< "\t\t\t\t\t\t\t\tto = \"materials/dev/dev_measuregeneric01b.vmat\"\n"
				<< "\t\t\t\t\t\t\t},\n"
				<< "\t\t\t\t\t\t]\n"
				<< "\t\t\t\t\t\tuse_global_default = false\n"
				<< "\t\t\t\t\t\tglobal_default_material = \"\"\n"
				<< "\t\t\t\t\t},\n"
				<< "\t\t\t\t]\n"
				<< "\t\t\t},\n"
				<< "\t\t\t{\n"
				<< "\t\t\t\t_class = \"RenderMeshList\"\n"
				<< "\t\t\t\tchildren = \n"
				<< "\t\t\t\t[\n"
				<< "\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t_class = \"RenderMeshFile\"\n"
				<< "\t\t\t\t\t\tfilename = \"models/src/" << bla.path().stem().string() << ".fbx\"\n"
				<< "\t\t\t\t\t\timport_translation = [ 0.0, 0.0, 0.0 ]\n"
				<< "\t\t\t\t\t\timport_rotation = [ 0.0, 0.0, 0.0 ]\n"
				<< "\t\t\t\t\t\timport_scale = 1.0\n"
				<< "\t\t\t\t\t\talign_origin_x_type = \"None\"\n"
				<< "\t\t\t\t\t\talign_origin_y_type = \"None\"\n"
				<< "\t\t\t\t\t\talign_origin_z_type = \"None\"\n"
				<< "\t\t\t\t\t\tparent_bone = \"\"\n"
				<< "\t\t\t\t\t\timport_filter =\n"
				<< "\t\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t\texclude_by_default = true\n"
				<< "\t\t\t\t\t\t\texception_list =\n"
				<< "\t\t\t\t\t\t\t[\n"
				<< "\t\t\t\t\t\t\t\t\"" << scene->getGeometry(0)->name << "\",\n"
				<< "\t\t\t\t\t\t\t]\n"
				<< "\t\t\t\t\t\t}\n"
				<< "\t\t\t\t\t},\n"
				<< "\t\t\t\t]\n"
				<< "\t\t\t},\n"
				<< "\t\t\t{\n"
				<< "\t\t\t\t_class = \"PhysicsShapeList\"\n"
				<< "\t\t\t\tchildren =\n"
				<< "\t\t\t\t[\n"
				<< "\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t_class = \"PhysicsMeshFile\"\n"
				<< "\t\t\t\t\t\tname = \"" << bla.path().stem().string() << "\"\n"
				<< "\t\t\t\t\t\tparent_bone = \"\"\n"
				<< "\t\t\t\t\t\tsurface_prop = \"default\"\n"
				<< "\t\t\t\t\t\tcollision_tags = \"solid\"\n"
				<< "\t\t\t\t\t\trecenter_on_parent_bone = false\n"
				<< "\t\t\t\t\t\toffset_origin = [ 0.0, 0.0, 0.0 ]\n"
				<< "\t\t\t\t\t\toffset_angles = [ 0.0, 0.0, 0.0 ]\n"
				<< "\t\t\t\t\t\talign_origin_x_type = \"None\"\n"
				<< "\t\t\t\t\t\talign_origin_y_type = \"None\"\n"
				<< "\t\t\t\t\t\talign_origin_z_type = \"None\"\n"
				<< "\t\t\t\t\t\tfilename = \"models/src/" << bla.path().stem().string() << ".fbx\"\n"
				<< "\t\t\t\t\t\timport_scale = 1.0\n"
				<< "\t\t\t\t\t\tmaxMeshVertices = 0\n"
				<< "\t\t\t\t\t\tqemError = 0.0\n"
				<< "\t\t\t\t\t\timport_filter =\n"
				<< "\t\t\t\t\t\t{\n"
				<< "\t\t\t\t\t\t\texcluded_by_default = false\n"
				<< "\t\t\t\t\t\t\texception_list = [ ]\n"
				<< "\t\t\t\t\t\t}\n"
				<< "\t\t\t\t\t},\n"
				<< "\t\t\t\t]\n"
				<< "\t\t\t},\n"
				<< "\t\t]\n"
				<< "\t\tmodel_archetype = \"\"\n"
				<< "\t\tprimary_associated_entity = \"\"\n"
				<< "\t\tanim_graph_name = \"\"\n"
				<< "\t\tbase_model_name = \"\"\n"
				<< "\t}\n"
				<< "}"

				;


			out.write(oss.str().c_str(), oss.str().size());

			out.close();

			delete scene; // to prevent a potentially MASSIVE memory leak :>
			// with this, the only limitation is the size of the file itself
			//(i would say ram is too, but 8GB is the norm today so it really
			//  shouldn't be an issue unless you intend to make it one :þ)

			++i;
		}
	}

	initOpenFBX("C:\\Projects\\gridlock\\models\\src\\srtm.fbx", hwnd);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use
	// ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your
	// application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling
	// ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double
	// backslash \\ !
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr,
	// io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != nullptr);

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT) done = true;
		}
		if (done) break;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		demoGUI();

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = {
			clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_pSwapChain->Present(1, 0); // Present with vsync
		// g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pd3dDevice,
		&featureLevel,
		&g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			createDeviceFlags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&sd,
			&g_pSwapChain,
			&g_pd3dDevice,
			&featureLevel,
			&g_pd3dDeviceContext);
	if (res != S_OK) return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = nullptr;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView)
	{
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite
// your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
// clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and hide them
// from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED) return 0;
			g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
			g_ResizeHeight = (UINT)HIWORD(lParam);
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY: ::PostQuitMessage(0); return 0;
		case WM_DPICHANGED:
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
			{
				// const int dpi = HIWORD(wParam);
				// printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
				const RECT* suggested_rect = (RECT*)lParam;
				::SetWindowPos(hWnd,
					nullptr,
					suggested_rect->left,
					suggested_rect->top,
					suggested_rect->right - suggested_rect->left,
					suggested_rect->bottom - suggested_rect->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
