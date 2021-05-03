#include <Cafe/ErrorHandling/CommonExceptions.h>
#include <Cafe/Misc/Scope.h>
#include <Cafe/TextUtils/Misc.h>
#include <cstring>

#if defined(_WIN32)
#	include <Windows.h>
#else
#	include <cerrno>
#endif

using namespace Cafe;
using namespace ErrorHandling;

SystemException::SystemException(Context context,
                                 Encoding::String<Encoding::CodePage::Utf8> description)
    : SystemException
{
	std::move(context),
#if defined(_WIN32)
	    GetLastError()
#else
	    errno
#endif
	        ,
	    std::move(description)
}
{
}

SystemException::SystemException(Context context, ErrorCodeType errorCode,
                                 Encoding::String<Encoding::CodePage::Utf8> description)
    : CafeException{ std::move(context), std::move(description) }, m_ErrorCode{ errorCode }
{
}

SystemException::~SystemException()
{
}

Encoding::StringView<Encoding::CodePage::Utf8> SystemException::GetErrorMessage() const
{
	if (m_ErrorMessage.IsEmpty())
	{
#if defined(_WIN32)
		LPVOID errMsg = nullptr;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		                   FORMAT_MESSAGE_IGNORE_INSERTS,
		               NULL, m_ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		               reinterpret_cast<LPWSTR>(&errMsg), 0, nullptr);
		if (errMsg)
		{
			CAFE_SCOPE_EXIT
			{
				LocalFree(errMsg);
			};

			m_ErrorMessage.Assign(TextUtils::EncodeFromWide<Encoding::CodePage::Utf8>(
			                          static_cast<const wchar_t*>(errMsg))
			                          .GetView());
		}
#else
		const auto errMsg = std::strerror(m_ErrorCode);
		m_ErrorMessage.Assign(
		    TextUtils::EncodeFromNarrow<Encoding::CodePage::Utf8>(errMsg).GetView());
#endif
	}

	return m_ErrorMessage.GetView();
}

SystemException::ErrorCodeType SystemException::GetErrorCode() const noexcept
{
	return m_ErrorCode;
}
