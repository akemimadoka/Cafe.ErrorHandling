#include <Cafe/ErrorHandling/ErrorHandling.h>

using namespace Cafe::ErrorHandling;

CafeException::CafeException(Context context,
                             Encoding::String<Encoding::CodePage::Utf8> description)
    : CafeException(
          std::move(context),
          std::make_shared<Encoding::String<Encoding::CodePage::Utf8>>(std::move(description)))
{
}

CafeException::CafeException(
    Context context,
    std::shared_ptr<Encoding::String<Encoding::CodePage::Utf8>> description) noexcept
    : m_Context{ std::move(context) }, m_Description
{
	std::move(description)
}
#if CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
, m_CaptureResult
{
	std::make_shared<StackWalker::CaptureResult>(StackWalker::Capture())
}
#endif
{
}

CafeException::~CafeException()
{
}

const char* CafeException::what() const noexcept
{
	return reinterpret_cast<const char*>(m_Description->GetData());
}

CafeException::Context const& CafeException::GetContext() const noexcept
{
	return m_Context;
}

#if CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION
std::shared_ptr<StackWalker::CaptureResult> const& CafeException::GetFrames() const noexcept
{
	return m_CaptureResult;
}
#endif
