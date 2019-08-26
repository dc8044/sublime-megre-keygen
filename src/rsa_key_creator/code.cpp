#include "rsakey.h"
#include "keyserialize.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc,char* argv[]) try
{
	if (argc == 2)
	{
		std::filesystem::path binaryPath(argv[1]);
		RSATool tool;
		auto privateKey = tool.GeneratePrivateKey();
		auto publicKey = tool.GeneratePublicKey();

		std::filesystem::path privateKeyFile(binaryPath / "privateKey.h");
		std::filesystem::path publicKeyFile(binaryPath / "publicKey.h");

		Serialize::save(privateKeyFile.string(), privateKey, "privateKey");
		Serialize::save(publicKeyFile.string(), publicKey, "publicKey");
	}

	return 0;
}
catch (const std::exception& error)
{
	std::cout << error.what() << std::endl;
}