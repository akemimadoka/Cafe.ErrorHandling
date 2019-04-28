#pragma once
#include <Cafe/Encoding/CodePage/UTF-8.h>
#include <Cafe/Misc/Utility.h>
#include <Cafe/Misc/UtilityMacros.h>
#include <exception>
#include <string_view>

#if CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
#	ifndef CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER
#		error Please enable CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER
#	endif
#	include "StackWalker.h"
#endif

namespace Cafe::ErrorHandling
{
	class CafeException : public std::exception
	{
	public:
		struct Context
		{
			std::string_view File;
			long long Line;
			std::string_view Func;
		};

		CafeException(Context context, Encoding::String<Encoding::CodePage::Utf8> description);
		CafeException(Context context,
		              std::shared_ptr<Encoding::String<Encoding::CodePage::Utf8>> description) noexcept;

		CafeException(CafeException const&) noexcept = default;
		CafeException(CafeException&&) noexcept = default;
		CafeException& operator=(CafeException const&) noexcept = default;
		CafeException& operator=(CafeException&&) noexcept = default;

		~CafeException() override;

		// 返回的是 Utf8 编码的字符串
		const char* what() const noexcept override;

		Context const& GetContext() const noexcept;

#if CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
		StackWalker::CaptureResult const& GetFrames() const noexcept;
#endif

	private:
		Context m_Context;
		std::shared_ptr<Encoding::String<Encoding::CodePage::Utf8>> m_Description;
#if CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
		StackWalker::CaptureResult m_CaptureResult;
#endif
	};
} // namespace Cafe::ErrorHandling

#define CAFE_THROW(exceptionClass, ...)                                                            \
	throw exceptionClass({ __FILE__, __LINE__, __func__ }, __VA_ARGS__)

#define CAFE_DEFINE_GENERAL_EXCEPTION(name, ...)                                                   \
	class name : public CAFE_EXPAND_OR(::Cafe::ErrorHandling::CafeException, __VA_ARGS__)            \
	{                                                                                                \
	public:                                                                                          \
		using BaseException = CAFE_EXPAND_OR(::Cafe::ErrorHandling::CafeException, __VA_ARGS__);       \
		using BaseException::BaseException;                                                            \
	}
