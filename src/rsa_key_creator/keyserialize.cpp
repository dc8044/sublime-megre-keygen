#include "keyserialize.h"

#include <fstream>
#include <iterator>

void Serialize::save(const std::string& filepath, const std::vector<uint8_t>& values,
	const std::string& paramName)
{		
	std::string data;

	for (auto it = values.cbegin();it != values.cend();++it)
	{
		char tmp[3] = { 0 };
		snprintf(tmp, sizeof(tmp), "%02X", *it);		
		data += std::string("0x") + std::string(tmp);

		if (it != std::prev(values.cend()))
		{
			data += ", ";
		}
	}

	auto content = std::string("#pragma once\n#include <cstdint>\n#include <array>\n") +
		std::string("constexpr std::array<uint8_t, ") +
		std::to_string(values.size()) + std::string("> ")  + paramName +
		std::string("= { ") + data + std::string("};");

	std::fstream output(filepath,std::ios::out);
	output << content;
	output.close();
}
