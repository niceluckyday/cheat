// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// IL2CPP application initializer

#include "pch-il2cpp.h"

#include "il2cpp-init.h"
#include "helpers.h"

#include <cheat/ILPatternScanner.h>
#include <cheat-base/config/field/StringField.h>

// IL2CPP APIs
#define DO_API(r, n, p) r (*n) p
#include "il2cpp-api-functions.h"
#undef DO_API

// Application-specific functions
#define DO_APP_FUNC(a, r, n, p) r (*n) p
#define DO_APP_FUNC_METHODINFO(a, n) struct MethodInfo ** n
namespace app {
#include "il2cpp-functions.h"
#include "il2cpp-unityplayer-functions.h"
}
#undef DO_APP_FUNC
#undef DO_APP_FUNC_METHODINFO

// TypeInfo pointers
#define DO_TYPEDEF(a, n) n ## __Class** n ## __TypeInfo
#define DO_SINGLETONEDEF(a, n) Singleton_1__Class** n ## __TypeInfo
namespace app {
#include "il2cpp-types-ptr.h"
}
#undef DO_TYPEDEF
#undef DO_SINGLETONEDEF

#define SELECT_OR(container, type, val, def) { auto value = val; if (value == 0) container = (type)(def); else container = (type)val; }

// IL2CPP application initializer
void init_il2cpp(const std::string& signaturePatterns)
{
	// Get base address of IL2CPP module
	uintptr_t baseAddress = il2cppi_get_base_address();

	using namespace app;

	static config::field::StringField m_OffsetData("OffsetData", "m_OffsetData", "PatternScanner", "{}");
	config::AddField(m_OffsetData);

	auto scanner = ILPatternScanner(signaturePatterns);
	scanner.Load(m_OffsetData.value());

	// Define IL2CPP API function addresses
	#define DO_API(r, n, p) n = (r (*) p)scanner.SearchAPI(#n);
	#include "il2cpp-api-functions.h"
	#undef DO_API

	il2cpp_thread_attach(il2cpp_domain_get());

	// Define function addresses
	#define DO_APP_FUNC(a, r, n, p) SELECT_OR(n, r (*) p, scanner.Search("UserAssembly.dll", #n), baseAddress + a)
 	#define DO_APP_FUNC_METHODINFO(a, n) SELECT_OR(n, struct MethodInfo **, scanner.SearchMethodInfo(#n), baseAddress + a)
	#include "il2cpp-functions.h"
	#undef DO_APP_FUNC
 	#undef DO_APP_FUNC_METHODINFO

	// Define TypeInfo variables
	#define DO_SINGLETONEDEF(a, n) SELECT_OR(n ## __TypeInfo, Singleton_1__Class**, scanner.SearchTypeInfo(#n), baseAddress + a)
	#define DO_TYPEDEF(a, n) SELECT_OR(n ## __TypeInfo, n ## __Class**, scanner.SearchTypeInfo(#n), baseAddress + a)
	#include "il2cpp-types-ptr.h"
	#undef DO_TYPEDEF
	#undef DO_SINGLETONEDEF

	uintptr_t unityPlayerAddress = il2cppi_get_unity_address();
	// Define UnityPlayer functions
    #define DO_APP_FUNC(a, r, n, p) SELECT_OR(n, r (*) p, scanner.Search("UnityPlayer.dll", #n), unityPlayerAddress + a)
    #include "il2cpp-unityplayer-functions.h"
    #undef DO_APP_FUNC

	scanner.Save(*m_OffsetData.valuePtr());
	m_OffsetData.Check();

	if (scanner.IsUpdated())
		LOG_INFO("Seems like some offsets was found for a first time. Recommend to restart game for correct cheat and game work.");
}

#undef SELECT_OR