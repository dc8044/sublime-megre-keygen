#include "rsakey.h"
#include <stdexcept>

std::vector<uint8_t> RSATool::GeneratePublicKey() const
{
	std::vector<uint8_t> result(1024);
	unsigned long outlen = result.size();

	if (rsa_export(reinterpret_cast<unsigned char*> (result.data()),
		&outlen, PK_PUBLIC, const_cast<rsa_key*> (&key)) != CRYPT_OK)
	{
		throw std::exception("Unable to save public key");
	}

	result.resize(outlen);
	return result;
}

std::vector<uint8_t> RSATool::GeneratePrivateKey() const
{
	std::vector<uint8_t> result(1024);
	unsigned long outlen = result.size();

	if (rsa_export(reinterpret_cast<unsigned char*> (result.data()),
		&outlen, PK_PRIVATE, const_cast<rsa_key*> (&key)) != CRYPT_OK)
	{
		throw std::exception("Unable to save private key");
	}

	result.resize(outlen);
	return result;
}

RSATool::RSATool()
{	
	static bool init = false;
	if (!init)
	{
		register_all_prngs();
		ltc_mp = ltm_desc;
		init = true;
	}

	if (rsa_make_key(nullptr, find_prng("sprng"),
		128, 17, &key) != CRYPT_OK)
	{
		throw std::exception("Bad rsa key creation");
	}
}
