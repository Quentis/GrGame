#pragma once
#include <string>

namespace Egg
{

	class HrException
	{
	public:
		std::string errorMessage;
		std::string filename;
		int lineNumber;
		HRESULT hr;

		HrException(HRESULT hr, std::string errorMessage, std::string filename, int lineNumber)
			:hr(hr), errorMessage(errorMessage), filename(filename), lineNumber(lineNumber)
		{
		}
	};

	class ThrowOnFail
	{
		std::string errorMessage;
		std::string filename;
		int lineNumber;
	public:
		ThrowOnFail(const char* errorMessage, const char* filename, int lineNumber)
			:errorMessage(errorMessage), filename(filename), lineNumber(lineNumber)
		{
		}

		void operator=(HRESULT hr)
		{
			if(FAILED(hr))
				throw HrException(hr, errorMessage, filename, lineNumber);
		}
	};

} // namespace Egg