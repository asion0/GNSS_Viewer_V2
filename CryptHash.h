#pragma once

namespace CryptHash
{
	//std::wstring CryptHash::HashFileSHA1(const std::wstring & filename);
	//std::wstring CryptHash::HashStringSHA1(const std::wstring & data);
	CString HashStringSHA1(LPCSTR data);
}