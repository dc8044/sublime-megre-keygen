#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>

class GlobalMemory
{
public:
	explicit GlobalMemory(SIZE_T size);
	~GlobalMemory() noexcept;
	HGLOBAL GetAllocationHandle() const noexcept;
	GlobalMemory() = delete;
	GlobalMemory(const GlobalMemory&) = delete;
	GlobalMemory(GlobalMemory&&) = delete;
	GlobalMemory& operator=(const GlobalMemory&) = delete;
	GlobalMemory& operator=(GlobalMemory&&) = delete;

private:
	HGLOBAL allocation;
};

class ClipBoard
{
public:
	explicit ClipBoard(HWND hwnd);
	~ClipBoard() noexcept;
	void SetTextToClipboard(const std::string& Text);
};