#include "peparser.h"

Parser::Parser(const std::string& PathToExe)
{
	pefile = std::make_unique<PeLib::PeFile64>(PathToExe);	
	pefile->readMzHeader();
	pefile->readPeHeader();
	peheader = std::make_unique<PeLib::PeHeader64>(pefile->peHeader());
}

std::vector<SectionData> Parser::getExecutableSections() const
{
	const unsigned int ExecutableCode = 0x20000000;
	std::vector<SectionData> result;

	for (size_t i = 0; i < peheader->calcNumberOfSections(); i++)
	{
		if (peheader->getCharacteristics(i) & ExecutableCode)
		{
			result.emplace_back(peheader->getPointerToRawData(i),
				peheader->getSizeOfRawData(i),
				peheader->getVirtualAddress(i),
				peheader->getVirtualSize(i));
		}
	}

	return std::forward<decltype(result)>(result);
}

uint64_t Parser::getImageBase() const
{
	return peheader->getImageBase();
}

uint64_t Parser::rawToRva(uint64_t rawValue) const
{
	return peheader->offsetToRva(rawValue);
}
