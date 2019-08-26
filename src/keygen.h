#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <privateKey.h>

extern "C"
{
#define LTC_SHA1
#define LTC_RIJNDAEL
#define LTC_SPRNG
#define LTM_DESC
#include <tomcrypt.h>
}

class CryptoInit
{
public:
	CryptoInit() = delete;
	~CryptoInit() = delete;
	CryptoInit(const CryptoInit&) = delete;
	CryptoInit(CryptoInit&&) = delete;
	CryptoInit& operator=(const CryptoInit&) = delete;
	CryptoInit& operator=(CryptoInit&&) = delete;

	static inline void Init()
	{
		register_all_ciphers();
		register_all_hashes();
		register_all_prngs();
		ltc_mp = ltm_desc;
	}
};

class Keygen
{
public:
	std::string GenerateLicenseKeyByName(const std::string& Name) const;
	std::string GetName() const;

private:
	uint32_t GenerateRandomSuffix() const;
	std::string MakeLicenseInfo(const std::string& UserName) const;
	std::vector<uint8_t> CreateSHA1Hash(const std::string& UserName) const;
	std::vector<uint8_t> SignHash(const std::vector<uint8_t>& hashValue) const;
	std::string SignatureToString(const std::vector<uint8_t>& signature) const;

	const char Separator = 0x0A;
};