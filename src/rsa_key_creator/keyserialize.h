#pragma once
#include <cstdint>
#include <string>
#include <vector>

class Serialize
{
public:
	static void save(const std::string& filepath, const std::vector<uint8_t>& values,
					const std::string& paramName);

	Serialize() = delete;
	~Serialize() = delete;
	Serialize(const Serialize&) = delete;
	Serialize(Serialize&&) = delete;
	Serialize& operator=(const Serialize&) = delete;
	Serialize& operator=(Serialize&&) = delete;
};