#include "disasm.h"
#include <stdexcept>
#include <string>

dis::Disassembler::Disassembler(uint64_t ImageBase, const std::vector<uint8_t>& binaryData):
	BaseOfImage(ImageBase),
	fileData(binaryData)
{
	auto status = ZydisDecoderInit(&decoder, 
		ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	if (ZYAN_FAILED(status))
	{
		throw std::exception("Unable to init disassembler engine");
	}

	status = ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	if (ZYAN_FAILED(status))
	{
		throw std::exception("Unable to init disassembler formatter");
	}
}

uint64_t dis::Disassembler::GetRawAddressOfCheckFunction(uint64_t initVectorVirtualAddress,
	const SectionData& section) const
{
	ZyanU64 analyzedBytes = section.RawData;	
	const ZyanU64 allBytesSize = section.RawData + section.RawSize;
	ZyanU64 runtime_address = section.VirtualAddress + BaseOfImage;
	std::string hexInitVectorVA = std::move(valueToStringHex(initVectorVirtualAddress));
	bool retnDetected = false;
	uint64_t result = analyzedBytes;

	while (analyzedBytes < allBytesSize)
	{
		ZydisDecodedInstruction instruction;
		auto status = ZydisDecoderDecodeBuffer(&decoder, fileData.data() + analyzedBytes,
			static_cast<ZyanUSize> (allBytesSize - analyzedBytes), &instruction);
		if (ZYAN_SUCCESS(status))
		{
			char buffer[256] = { 0 };
			ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
				runtime_address);

			const auto testOfInstruction = std::string(buffer);
			if (testOfInstruction.find("ret") != std::string::npos)
			{
				retnDetected = true;
			}
			else if (retnDetected)
			{
				retnDetected = false;
				result = analyzedBytes;
			}

			if (testOfInstruction.find(hexInitVectorVA) !=
				std::string::npos)
			{
				return result;
			}

			analyzedBytes += instruction.length;
			runtime_address += instruction.length;
		}
		else
		{
			analyzedBytes++;
			runtime_address++;
		}
	}

	throw std::exception("Unable to find address in this section");
}