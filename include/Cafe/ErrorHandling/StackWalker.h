#pragma once

#include <Cafe/Encoding/CodePage/UTF-8.h>
#include <Cafe/ErrorHandling/Config/ErrorHandlingConfig.h>
#include <vector>

#if CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER

#	if defined(_WIN32)
#		include <Windows.h>
#		ifdef _MSC_VER
#			pragma warning(push)
#			pragma warning(disable : 4091)
#		endif
#		include <DbgHelp.h>
#		ifdef _MSC_VER
#			pragma warning(pop)
#		endif
#	elif defined(__linux__)
#		include <execinfo.h>
#	else // 由于此处已经判断，以下 #else 逻辑都将视为 linux
#		error Unsupported platform, Please turn off option CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER
#	endif

namespace Cafe::ErrorHandling::StackWalker
{
	constexpr std::size_t MaxCaptureFrames =
#	ifdef _WIN32
#		if WINVER <= _WIN32_WINNT_WS03
	    62
#		else
	    std::numeric_limits<USHORT>::max()
#		endif
#	else
	    255
#	endif
	    ;

#	ifdef _WIN32
	using AddressType = PVOID;
	using SymbolAddressType = decltype(SYMBOL_INFO::Address);
	using SourceFileAddressType = decltype(IMAGEHLP_LINE64::Address);
	using SourceFileLineNumberType = decltype(IMAGEHLP_LINE64::LineNumber);
#	else
	using AddressType = void*;
#	endif

	struct Frame
	{
		AddressType Address;
#	ifdef _WIN32
		Encoding::String<Encoding::CodePage::Utf8> SymbolName;
		SymbolAddressType SymbolAddress;
		Encoding::String<Encoding::CodePage::Utf8> SourceFileName;
		SourceFileAddressType SourceFileAddress;
		SourceFileLineNumberType SourceFileLine;
#	else
		Encoding::String<Encoding::CodePage::Utf8> Description;
#	endif
	};

	struct CaptureResult
	{
		std::vector<Frame> Frames;
	};

#	ifdef _WIN32
	CaptureResult Capture(
	    std::size_t captureFrames = MaxCaptureFrames, std::size_t skipFrames = 0,
	    Encoding::StringView<Encoding::CodePage::Utf8> unknownSymbolName = CAFE_UTF8_SV("Unknown"),
	    Encoding::StringView<Encoding::CodePage::Utf8> unknownFileName = CAFE_UTF8_SV("Unknown"));
#	else
	CaptureResult Capture(
	    std::size_t captureFrames = MaxCaptureFrames,
	    Encoding::StringView<Encoding::CodePage::Utf8> unknownDescription = CAFE_UTF8_SV("Unknown"));
#	endif
} // namespace Cafe::ErrorHandling::StackWalker

#endif
