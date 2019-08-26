#include "clip.h"

#include <stdexcept>

GlobalMemory::GlobalMemory(SIZE_T size)
{
	allocation = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!allocation)
	{
		throw std::exception("Unable to allocate global memory");
	}
}

GlobalMemory::~GlobalMemory() noexcept
{
	GlobalFree(allocation);
}

HGLOBAL GlobalMemory::GetAllocationHandle() const noexcept
{
	return allocation;
}

ClipBoard::ClipBoard(HWND hwnd)
{
	if (!OpenClipboard(hwnd))
	{
		throw std::exception("Unable to open clipboard");
	}

	if (!EmptyClipboard())
	{
		CloseClipboard();
		throw std::exception("Unable to clear clipboard");
	}
}

ClipBoard::~ClipBoard() noexcept
{
	CloseClipboard();
}

void ClipBoard::SetTextToClipboard(const std::string& Text)
{	
	const auto size = Text.length() + 
		sizeof(std::remove_reference_t<decltype(Text)>::value_type);
	GlobalMemory gm(size);

	auto* buffer = static_cast<char*> (GlobalLock(gm.GetAllocationHandle()));
	if (!buffer)
	{
		throw std::exception("Unable to lock global memory");
	}

	memcpy(buffer, Text.data(), size);
	GlobalUnlock(buffer);
	SetClipboardData(CF_TEXT, reinterpret_cast<HANDLE> (gm.GetAllocationHandle()));
}
