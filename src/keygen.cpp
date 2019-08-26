#include "keygen.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Windows.h>

std::vector<uint8_t> Keygen::CreateSHA1Hash(const std::string& licenseData) const
{
	const unsigned int sha1Len = 20;		
	const auto hashId = find_hash("sha1");
	std::vector<uint8_t> result(sha1Len);
	unsigned long size = result.size();

	auto cryptResult = hash_memory(hashId, (const unsigned char*) (licenseData.data()),
		static_cast<unsigned long> (licenseData.size()), result.data(), &size);

	if (cryptResult != CRYPT_OK)
	{
		auto message = std::string("Error in hash calculation: ") +
			std::to_string(cryptResult);
		throw std::exception(message.c_str());
	}

	return result;
}

std::vector<uint8_t> Keygen::SignHash(const std::vector<uint8_t>& hashValue) const
{
	rsa_key key;
	std::vector<uint8_t> result(500, 0);
	unsigned long size = result.size();

	if (auto res = rsa_import(privateKey.data(), 
		static_cast<unsigned long> (privateKey.size()),
		&key) != CRYPT_OK)
	{
		auto message = std::string("Unable to import key, code: ") +
			std::to_string(res);
		throw std::exception(message.c_str());
	}
	
	if (auto res = rsa_sign_hash_ex(hashValue.data(), hashValue.size(), result.data(),
		&size, LTC_PKCS_1_V1_5, nullptr, 0, find_hash("sha1"), 0, &key) != CRYPT_OK)
	{
		rsa_free(&key);
		auto message = std::string("Unable to sign data, code: ") +
			std::to_string(res);
		throw std::exception(message.c_str());
	}

	result.resize(size);
	rsa_free(&key);
	return std::forward<decltype(result)> (result);
}

std::string Keygen::SignatureToString(const std::vector<uint8_t>& signature) const
{
	std::string result;
	int symbolsOnLine = 0;
	const int symbolsPerLine = 16;

	for (const auto& element : signature)
	{
		if (symbolsOnLine == symbolsPerLine)
		{
			symbolsOnLine = 0;
			result += Separator;
		}

		char tmp[3] = { 0 };
		snprintf(tmp, sizeof(tmp), "%02X", element);
		result += std::string(tmp);	
		++symbolsOnLine;
	}
	
	return std::forward<decltype(result)> (result);
}

std::string Keygen::GenerateLicenseKeyByName(const std::string& Name) const
{
	auto licenseInfo = MakeLicenseInfo(Name);
	auto hash = CreateSHA1Hash(licenseInfo);
	auto sign = SignHash(hash);	
	auto printableSignature = SignatureToString(sign);
	return licenseInfo + static_cast<char> (0x0A) + printableSignature;
}

std::string Keygen::GetName() const
{
	constexpr DWORD startLenght = 100;
	constexpr unsigned int MaxNameLength = 17;
	std::vector<char> buffer(startLenght, 0);
	DWORD bufferLength = buffer.size();

	while (!GetUserNameA(reinterpret_cast<LPSTR> (buffer.data()), &bufferLength))
	{
		bufferLength = buffer.size() * 2;
		buffer.resize(bufferLength);
		memset(buffer.data(), 0, buffer.size());
	}

	const auto realLength = strlen(buffer.data());
	if (realLength > MaxNameLength)
	{
		return std::forward<std::string>(std::string(buffer.cbegin(), 
			buffer.cbegin() + MaxNameLength));
	}

	return std::forward<std::string>(std::string(buffer.cbegin(), 
		buffer.cbegin() + realLength));
}

uint32_t Keygen::GenerateRandomSuffix() const
{
	std::srand(static_cast<unsigned int> (std::time(0)));
	return static_cast<uint32_t> (std::rand());
}

std::string Keygen::MakeLicenseInfo(const std::string& UserName) const
{
	const std::string Prefix("E52D");
	const std::string LicenseType = "Unlimited User License";	
	const auto suffix = GenerateRandomSuffix();

	return UserName + Separator + LicenseType + Separator +
		Prefix + '-' + std::to_string(suffix);
}
