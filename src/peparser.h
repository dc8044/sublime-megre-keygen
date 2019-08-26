#pragma once
#include "section.h"

#include <PeHeader.h>
#include <PeFile.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Parser
{
public:
	explicit Parser(const std::string& PathToExe);
	std::vector<SectionData> getExecutableSections() const;
	uint64_t getImageBase() const;
	uint64_t rawToRva(uint64_t rawValue) const;
	Parser() = delete;
	~Parser() = default;
	Parser(const Parser&) = delete;
	Parser(Parser&&) = delete;
	Parser& operator=(const Parser&) = delete;
	Parser& operator=(Parser&&) = delete;

private:
	std::unique_ptr<PeLib::PeFile64> pefile;
	std::unique_ptr<PeLib::PeHeader64> peheader;
};