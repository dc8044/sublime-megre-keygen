#pragma once
#include <cstdint>
#include <vector>

extern "C"
{
#define LTC_SPRNG
#define LTM_DESC
#include <tommath.h>
#include <tomcrypt.h>
}

class RSATool
{
public:
	std::vector<uint8_t> GeneratePublicKey() const;
	std::vector<uint8_t> GeneratePrivateKey() const;
	RSATool();
	~RSATool() = default;
	RSATool(const RSATool&) = delete;
	RSATool(RSATool&&) = delete;
	RSATool& operator=(const RSATool&) = delete;
	RSATool& operator=(RSATool&&) = delete;

private:
	rsa_key key = { 0 };
};