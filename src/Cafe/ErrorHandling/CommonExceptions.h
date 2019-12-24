#pragma once

#include "ErrorHandling.h"

namespace Cafe::ErrorHandling
{
	CAFE_DEFINE_GENERAL_EXCEPTION(NotImplementedException);

	class CAFE_PUBLIC SystemException : public CafeException
	{
	public:
#if defined(_WIN32)
		using ErrorCodeType = std::uint32_t;
#else
		using ErrorCodeType = int;
#endif

		SystemException(Context context, Encoding::String<Encoding::CodePage::Utf8> description);

		SystemException(Context context, ErrorCodeType errorCode,
		                Encoding::String<Encoding::CodePage::Utf8> description);

		~SystemException();

		Encoding::StringView<Encoding::CodePage::Utf8> GetErrorMessage() const;
		ErrorCodeType GetErrorCode() const noexcept;

	private:
		ErrorCodeType m_ErrorCode;
		mutable Encoding::String<Encoding::CodePage::Utf8> m_ErrorMessage;
	};
} // namespace Cafe::ErrorHandling
