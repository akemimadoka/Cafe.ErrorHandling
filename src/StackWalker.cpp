#include <Cafe/ErrorHandling/ErrorHandling.h>
#include <Cafe/ErrorHandling/StackWalker.h>
#include <Cafe/TextUtils/Misc.h>

#ifdef _WIN32
#	include <mutex>
#endif

using namespace Cafe;
using namespace ErrorHandling;

#ifdef _WIN32

namespace
{
	std::once_flag s_SymInitFlag{};
	void SymInit()
	{
		std::call_once(s_SymInitFlag, [] {
			SymSetOptions(SYMOPT_LOAD_LINES);
			if (!SymInitialize(GetCurrentProcess(), nullptr, TRUE))
			{
				CAFE_THROW(CafeException, CAFE_UTF8_SV("SymInitialize failed."));
			}

			std::atexit([] { SymCleanup(GetCurrentProcess()); });
		});
	}
} // namespace

StackWalker::CaptureResult
StackWalker::Capture(std::size_t captureFrames, std::size_t skipFrames,
                     Encoding::StringView<Encoding::CodePage::Utf8> unknownSymbolName,
                     Encoding::StringView<Encoding::CodePage::Utf8> unknownFileName)
{
#	if WINVER <= _WIN32_WINNT_WS03
	if (captureFrames >= 63 || skipFrames >= 63 - captureFrames)
	{
		CAFE_THROW(
		    CafeException,
		    CAFE_UTF8_SV(
		        "captureFrames or skipFrames is too large for Windows Server 2003 / Windows XP, see "
		        "https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633(v=vs.85).aspx ."));
	}
#	endif

	if (skipFrames > captureFrames)
	{
		CAFE_THROW(CafeException, CAFE_UTF8_SV("Skipped too many frames."));
	}

	SymInit();

	const auto hProcess = GetCurrentProcess();
	static thread_local PVOID s_Stack[MaxCaptureFrames];
	const auto frames =
	    CaptureStackBackTrace(static_cast<DWORD>(skipFrames),
	                          static_cast<DWORD>(captureFrames - skipFrames), s_Stack, nullptr);

	struct
	{
		SYMBOL_INFO SymbolInfo;
		char Name[MAX_SYM_NAME];
	} symbol;
	symbol.SymbolInfo.SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol.SymbolInfo.MaxNameLen = MAX_SYM_NAME;
	IMAGEHLP_LINE64 line{};
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	DWORD displacement;

	StackWalker::CaptureResult result;
	result.Frames.reserve(frames);

	for (std::size_t i = 0; i < frames; ++i)
	{
		const auto address = s_Stack[i];
		const auto [symbolName, symbolAddress] = [&] {
			if (SymFromAddr(hProcess, reinterpret_cast<DWORD64>(address), nullptr, &symbol.SymbolInfo))
			{
				return std::pair{ Cafe::TextUtils::EncodeFromNarrow<Cafe::Encoding::CodePage::Utf8>(
					                    { symbol.SymbolInfo.Name, symbol.SymbolInfo.NameLen }),
					                symbol.SymbolInfo.Address };
			}

			return std::pair{ unknownSymbolName.ToString(), SymbolAddressType{} };
		}();

		const auto [sourceFileName, sourceFileAddress, sourceFileLine] = [&] {
			if (SymGetLineFromAddr64(hProcess, reinterpret_cast<DWORD64>(s_Stack[i]), &displacement,
			                         &line))
			{
				return std::tuple{ Cafe::TextUtils::EncodeFromNarrow<Cafe::Encoding::CodePage::Utf8>(
					                     { line.FileName }),
					                 line.Address, line.LineNumber };
			}

			return std::tuple{ unknownFileName.ToString(), SourceFileAddressType{},
				                 SourceFileLineNumberType{} };
		}();

		result.Frames.push_back({ s_Stack[i], std::move(symbolName), symbolAddress,
		                          std::move(sourceFileName), sourceFileAddress, sourceFileLine });
	}

	return result;
}
#else
StackWalker::CaptureResult
StackWalker::Capture(std::size_t captureFrames,
                     Encoding::StringView<Encoding::CodePage::Utf8> unknownDescription)
{
	static thread_local AddressType s_Stack[MaxCaptureFrames];
	const auto size = static_cast<std::size_t>(backtrace(s_Stack, static_cast<int>(captureFrames)));
	const auto symbolInfo = std::unique_ptr<char*[], void (*)(char**)>(
	    backtrace_symbols(s_Stack, static_cast<int>(size)), +[](char** p) { std::free(p); });

	StackWalker::CaptureResult result;
	for (std::size_t i = 0; i < size; ++i)
	{
		const auto symInfo = symbolInfo[i];
		result.Frames.push_back(
		    { s_Stack[i], symInfo ? Cafe::TextUtils::EncodeFromNarrow<Encoding::CodePage::Utf8>(symInfo)
		                          : unknownDescription.ToString() });
	}
	return result;
}
#endif
