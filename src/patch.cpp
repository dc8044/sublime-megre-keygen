#include "patch.h"
#include "disasm.h"
#include <publicKey.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <Windows.h>

patch::patch(const std::string& Path):
	peparser(Path)
{
	std::ifstream input(Path,std::ios::binary);

	if (!input.is_open())
	{
		auto message = "Unable to open file " + Path;
		throw std::exception(message.c_str());
	}

	input.seekg(0, input.end);
	const auto fileSize = static_cast<unsigned int> (input.tellg());
	input.seekg(0, input.beg);
	fileData.resize(fileSize);

	input.read(reinterpret_cast<char*> (fileData.data()), fileData.size());
	if (!input)
	{
		throw std::exception("Unable to read the whole file");
	}
}

void patch::Replace(const std::vector<uint8_t>& data, const std::vector<uint8_t>& patched, 
	const std::string& resultFileName)
{
	if (data.size() != patched.size())
	{
		throw std::exception("Data should be patched with the same size");
	}

	auto patchPlace = std::search(fileData.cbegin(), fileData.cend(), data.cbegin(), data.cend());
	if (patchPlace == fileData.cend())
	{
		throw std::exception("Data not found in the target, nothing to patch");
	}

	auto result = std::vector<uint8_t>(fileData.cbegin(), patchPlace);
	std::copy(patched.cbegin(), patched.cend(), std::back_inserter(result));
	auto tmp = std::vector<uint8_t>(patchPlace + patched.size(), fileData.cend());
	std::copy(tmp.cbegin(), tmp.cend(), std::back_inserter(result));

	std::ofstream output(resultFileName, std::ios::binary);
	output.write(const_cast<const char*> (reinterpret_cast<char*> (result.data())), result.size());
	output.close();
}

std::vector<uint8_t> patch::GetOldPublicKey() const
{
	return std::vector<uint8_t>(publicOldKey.cbegin(), publicOldKey.cend());
}

std::vector<uint8_t> patch::GetNewPublicKey() const
{
	return std::vector<uint8_t>(publicKey.cbegin(), publicKey.cend());
}

void patch::FixPublicKey()
{
	std::string pathToExe = patch::GetFileToPatch();
	patch Patcher(pathToExe);
	Patcher.FixPublicKeyPatchCheck();

	auto oldKey = Patcher.GetOldPublicKey();
	auto newKey = Patcher.GetNewPublicKey();
	Patcher.Replace(oldKey, newKey, pathToExe);	
}

void patch::FixPublicKeyPatchCheck()
{
	const uint64_t initVectorPosition = FindSha256SignatureRawPosition();
	const uint64_t checkProcPosition = GetRawSha256Address(initVectorPosition);
	PatchSha256Check(checkProcPosition);
}

std::string patch::GetFileToPatch()
{		
	char buffer[MAX_PATH] = { 0 };
	OPENFILENAMEA fileName = { 0 };
	fileName.lStructSize = sizeof(decltype(fileName));
	fileName.lpstrFilter = "All Files\0*.*\0\0";
	fileName.nMaxFile = sizeof(buffer);
	fileName.lpstrFile = buffer;

	if (GetOpenFileNameA(&fileName))
	{
		return std::forward<std::string>(std::string(fileName.lpstrFile));
	}

	throw std::exception("Unable to open file for patching");
}

uint64_t patch::FindSha256SignatureRawPosition() const
{
	auto patchPlace = std::search(fileData.cbegin(), fileData.cend(), 
		sha256signature.cbegin(), sha256signature.cend());

	if (patchPlace == fileData.cend())
	{
		throw std::exception("Unable to find init vector for sha256");
	}

	return static_cast<uint64_t> (std::distance(fileData.cbegin(), patchPlace));
}

uint64_t patch::GetRawSha256Address(uint64_t InitVectorAddress) const
{	
	const auto sections = peparser.getExecutableSections();
	const auto ImageBase = peparser.getImageBase();
	const auto InitVectorVirtualAddress = peparser.rawToRva(InitVectorAddress) + ImageBase;
	dis::Disassembler disasm(ImageBase, fileData);

	for (const auto& section : sections)
	{
		try {
			return disasm.GetRawAddressOfCheckFunction(InitVectorVirtualAddress, section);
		}
		catch (const std::exception&)
		{
			continue;
		}
	}

	throw std::exception("Unable to find reference to sha256 init vector");
}

void patch::PatchSha256Check(uint64_t CheckOffset)
{
	static const std::array<uint8_t, 11> modifiedCode{
		0xC6, 0x41, 0x0F, 0xA8,  //mov byte ptr [rcx+F], 0xA8
		0xC6, 0x41, 0x1E, 0x56,  //mov byte ptr [rcx+1E], 0x56
		0xC3					 //ret
	};

	auto start = fileData.begin() + (const int) CheckOffset;
	for (const auto& v : modifiedCode)
	{
		*start = v;
		++start;
	}
}
