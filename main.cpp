#include <cuchar>
#include "sciter-x-window.hpp"
#include "sciter-x.h"
#include "resources.cpp"

#if defined(_WIN32) || defined(_WIN64)
#include <shellapi.h>
#endif

std::string writing = "running";

class frame : public sciter::window
{
public:
	frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

	// map of native functions exposed to script:
	BEGIN_FUNCTION_MAP
		FUNCTION_0("nativeMessage", nativeMessage);
		FUNCTION_1("launch_command", launch_command);
		FUNCTION_1("launch_url", launch_url);
	END_FUNCTION_MAP
	// function expsed to script:
	sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

	sciter::string launch_url(const sciter::value& url)
	{
#if defined(_WIN32) || defined(_WIN64)
        launch_command(url);
#else if defined(LINUX)
        auto x = u"xdg-open " + url.to_string();
        std::cout << typeid(x).name();
        system(w2t(x));
        return WSTR("Hmm");
#endif
	}

	sciter::string launch_command(const sciter::value& app_name)
	{
#if defined(_WIN32) || defined(_WIN64)
		ShellExecuteW(NULL, L"open", app_name.to_string().c_str(), NULL, NULL, SW_SHOWNORMAL);
#else if defined(LINUX)
		system(w2t(app_name.to_string()));
#endif
		return WSTR("Done");
	}
};

int uimain(std::function<int()> run) {

	// enable features you may need in your scripts:
	SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
		ALLOW_FILE_IO |
		ALLOW_SOCKET_IO | // NOTE: the must for communication with Inspector
		ALLOW_EVAL |
		ALLOW_SYSINFO);

	sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

	aux::asset_ptr<frame> pwin = new frame();

	// note: this:://app URL is dedicated to the sciter::archive content associated with the application
	pwin->load(WSTR("this://app/index.html"));
	//or use this to load UI from
	//  pwin->load( WSTR("file:///home/andrew/Desktop/Project/res/main.htm") );

	pwin->expand();

	return run();
}
