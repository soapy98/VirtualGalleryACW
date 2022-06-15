#include "D3DFramework.h"

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE const hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int const nShowCmd) {

	auto& app = D3DFramework::GetInstance();

	if (FAILED(app.InitWindow(hInstance, nShowCmd)))
		return 0;

	if (FAILED(app.InitDevice())) 
		return 0;

	// Main message loop
	MSG msg;
	msg.message = 0;
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			app.UpdateView();
			app.Render();
			
		}
	}

	return static_cast<int>(msg.wParam);
}