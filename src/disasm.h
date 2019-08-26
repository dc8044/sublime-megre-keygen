#pragma once
#include "section.h"

#define ZYDIS_STATIC_DEFINE
#include <Zydis/Zydis.h>

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <vector>

namespace dis
{
	class Disassembler
	{
	public:
		explicit Disassembler(uint64_t ImageBase, const std::vector<uint8_t>& binaryData);
		uint64_t GetRawAddressOfCheckFunction(uint64_t initVectorVirtualAddress, 
			const SectionData& section) const;
		Disassembler() = delete;
		~Disassembler() = default;
		Disassembler(const Disassembler&) = delete;
		Disassembler(Disassembler&&) = delete;
		Disassembler& operator=(const Disassembler&) = delete;
		Disassembler& operator=(Disassembler&&) = delete;

	private:
		uint64_t BaseOfImage;
		const std::vector<uint8_t>& fileData;
		ZydisDecoder decoder;
		ZydisFormatter formatter;

		template <typename T>
		inline std::string valueToStringHex(T value) const
		{
			std::stringstream stream;
			stream << std::hex << value;
			auto temp = stream.str();
			std::for_each(temp.begin(), temp.end(), [](char& symbol) -> void
			{
				symbol = ::toupper(symbol);
			});
			return temp;
		}
	};
}