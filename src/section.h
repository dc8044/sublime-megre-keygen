#pragma once
#include <cstdint>

struct SectionData
{
	uint64_t RawData;
	uint64_t RawSize;
	uint64_t VirtualAddress;
	uint64_t VirtualSize;
	SectionData(uint64_t RD, uint64_t RS, uint64_t VA, uint64_t VS) :
		RawData(RD),
		RawSize(RS),
		VirtualAddress(VA),
		VirtualSize(VS)
	{ }
};