// PackMNG.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <vector>

#define _TAG4(s) ( \
                (((s) >> 24) & 0xFF)       | \
                (((s) >> 8 ) & 0xFF00)     | \
                (((s) << 24) & 0xFF000000) | \
                (((s) << 8 ) & 0x00FF0000) \
                )
#define TAG4(s) _TAG4((DWORD)(s))

#define swab32(x) ((x&0x000000ff) << 24 | (x&0x0000ff00) << 8 | (x&0x00ff0000) >> 8 | (x&0xff000000) >> 24)

struct iChunk
{
	ULONG m_Mask, m_CRC;

	virtual void  SetMask(ULONG Mask)
	{
		m_Mask = Mask;
	}
	virtual ULONG GetMask()
	{
		return m_Mask;
	}
	virtual void  SetData(PBYTE Data, ULONG Size) = 0;
	virtual void  GetData(PBYTE& Data, ULONG& Size) = 0;
	virtual void  SetCRC(ULONG CRC)
	{
		m_CRC = CRC;
	}
	virtual ULONG GetCRC()
	{
		return m_CRC;
	}
};

struct PNGChunk : public iChunk
{
	ULONG m_Size;
	PBYTE m_Buffer;
	std::string m_Name;

	void  SetData(PBYTE Data, ULONG Size)
	{
		m_Buffer = Data;
		m_Size   = Size;
	}
	void  GetData(PBYTE& Data, ULONG& Size)
	{
		Size = m_Size;
		Data = m_Buffer;
	}

	std::string GetName()
	{
		return m_Name;
	}

	void Parse()
	{
		ULONG iPos = 9;
		m_Name = (LPCSTR)(m_Buffer + iPos);
	}

	void ReplacePNG(PBYTE Buffer, ULONG Size)
	{
		ULONG Length = *(PDWORD)m_Buffer;
		Length = swab32(Length);
		Length += 12;

		ULONG Offset = Length;
		ULONG NextLength = *(PDWORD)(m_Buffer + Offset);
		NextLength = swab32(NextLength);
		Offset += 4;
		ULONG NextMask = *(PDWORD)(m_Buffer + Offset);
		Offset += 4;

		if (NextMask == TAG4('DEFI'))
		{
			Length += NextLength + 12;
		}


		PBYTE NewBuffer = new BYTE[Size - 8 + Length + MAX_PATH + 12];
		memcpy(NewBuffer, m_Buffer, Length);

		ULONG vOffset = 0;
		ULONG TextLength = 0;

		ULONG ThisLength, ThisMask;

		BOOL FoundTextChunk = FALSE;
		ThisLength = *(PDWORD)(m_Buffer + vOffset);
		ThisLength = swab32(ThisLength);
		vOffset += 4;
		ThisMask = *(PDWORD)(m_Buffer + vOffset);
		vOffset += 4;

		while (vOffset < m_Size)
		{
			if (ThisMask == TAG4('tEXt'))
			{
				FoundTextChunk = TRUE;
				break;
			}
			else
			{
				vOffset += ThisLength;
				vOffset += 4; //crc
				ThisLength = *(PDWORD)(m_Buffer + vOffset);
				ThisLength = swab32(ThisLength);
				vOffset += 4;
				ThisMask = *(PDWORD)(m_Buffer + vOffset);
				vOffset += 4;
			}
		}

		if (!FoundTextChunk)
		{
			memcpy(NewBuffer + Length, Buffer + 8, Size - 8);
			m_Size = Size - 8 + Length;

			printf("Warning : Couldn't find text chunk...\n");
		}
		else
		{
			vOffset -= 8;
			ThisLength += 12;

			ULONG FirstLength = *(PDWORD)(Buffer + 8);
			FirstLength = swab32(FirstLength);

			memcpy(NewBuffer + Length, Buffer + 8, FirstLength + 12);
			memcpy(NewBuffer + Length + FirstLength + 12, m_Buffer + vOffset, ThisLength);
			memcpy(NewBuffer + Length + FirstLength + 12 + ThisLength, Buffer + 8 + FirstLength + 12, Size - 8 - (FirstLength + 12));

			m_Size = Size - 8 + Length + ThisLength;
		}

		
		m_Buffer = NewBuffer;
	}
};


struct NormalChunk : public iChunk
{
	ULONG m_Size;
	PBYTE m_Buffer;

	void  SetData(PBYTE Data, ULONG Size)
	{
		m_Buffer = Data;
		m_Size = Size;
	}
	void  GetData(PBYTE& Data, ULONG& Size)
	{
		Size = m_Size;
		Data = m_Buffer;
	}
};




int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		return 0;
	}

	FILE* file = _wfopen(argv[1], L"rb");
	if (!file)
	{
		return 0;
	}

	PBYTE Buffer;
	ULONG Size;

	fseek(file, 0, SEEK_END);
	Size = ftell(file);
	rewind(file);
	Buffer = new BYTE[Size];
	fread(Buffer, 1, Size, file);
	fclose(file);

	BYTE Header[8];
	ULONG iPos = 0;

	memcpy(Header, Buffer + iPos, 8);
	iPos += 8;

	std::vector<iChunk*> ChunkList;
	while (iPos < Size)
	{
		DWORD Length = *(PDWORD)(Buffer + iPos);
		Length = swab32(Length);
		iPos += 4;
		DWORD Mask = *(PDWORD)(Buffer + iPos);
		iPos += 4;

		if (Mask == TAG4('FRAM'))
		{
			ULONG CurOffset = iPos - 8;

			while (Mask != TAG4('IEND'))
			{
				iPos += Length;
				iPos += 4; //crc
				Length = *(PDWORD)(Buffer + iPos);
				Length = swab32(Length);
				iPos += 4; //next length
				Mask = *(PDWORD)(Buffer + iPos);
				iPos += 4;
			}

			iPos += Length;
			iPos += 4;

			PNGChunk* Chunk = new PNGChunk();
			Chunk->SetData(Buffer + CurOffset, iPos - CurOffset);
			Chunk->SetMask(0x23333333);
			Chunk->Parse();

			ChunkList.push_back(Chunk);
		}
		else
		{
			iChunk* Chunk = new NormalChunk();
			Chunk->SetMask(Mask);
			Chunk->SetData((Buffer + iPos), Length);
			iPos += Length;
			DWORD Crc = *(PDWORD)(Buffer + iPos);
			iPos += 4;
			Chunk->SetCRC(Crc);
			
			ChunkList.push_back(Chunk);
		}
	}

	ULONG Index = 0;
	for (auto Chunk : ChunkList)
	{
		if (Chunk->GetMask() == 0x23333333)
		{
			std::wstring FileNameFull(argv[2]);
			WCHAR FileName[MAX_PATH] = { 0 };
			auto Name = ((PNGChunk*)Chunk)->GetName();

			if (Name.length())
			{
				MultiByteToWideChar(932, 0, Name.c_str(), Name.length(), FileName, MAX_PATH);

				wprintf(L"Linking... %s\n", FileName);
				lstrcatW(FileName, L".png");
				FileNameFull += L"\\";
				FileNameFull += FileName;

				if (GetFileAttributesW(FileNameFull.c_str()) == 0xffffffff)
				{
					FileNameFull = argv[2];
					FileNameFull += L"\\";
					wsprintfW(FileName, L"%04d.png", Index);
					FileNameFull += FileName;
				}
			}
			else
			{
				FileNameFull += L"\\";
				wsprintfW(FileName, L"%04d.png", Index);
				FileNameFull += FileName;
			}

			FILE* Reader = _wfopen(FileNameFull.c_str(), L"rb");
			if (Reader == nullptr)
			{
				wprintf(L"Failed to load %s\n", FileNameFull.c_str());
				return 0;
			}

			ULONG FileSize;
			fseek(Reader, 0, SEEK_END);
			FileSize = ftell(Reader);
			rewind(Reader);
			PBYTE FileBuffer = new BYTE[FileSize];
			fread(FileBuffer, 1, FileSize, Reader);
			fclose(Reader);

			((PNGChunk*)Chunk)->ReplacePNG(FileBuffer, FileSize);

			Index++;
		}
	}


	std::wstring OutFileName(argv[1]);
	OutFileName += L".out";

	FILE* Writer = _wfopen(OutFileName.c_str(), L"wb");
	fwrite(Header, 1, 8, Writer);
	for (auto Chunk : ChunkList)
	{
		ULONG vSize, vLength, vMask, vCrc;
		PBYTE vBuffer;
		if (Chunk->GetMask() == 0x23333333)
		{
			((PNGChunk*)Chunk)->GetData(vBuffer, vSize);
			fwrite(vBuffer, 1, vSize, Writer);
		}
		else
		{
			vMask = Chunk->GetMask();
			vCrc = Chunk->GetCRC();
			Chunk->GetData(vBuffer, vSize);

			vSize = swab32(vSize);
			fwrite(&vSize, 1, 4, Writer);
			fwrite(&vMask, 1, 4, Writer);

			vSize = swab32(vSize);
			fwrite(vBuffer, 1, vSize, Writer);
			fwrite(&vCrc, 1, 4, Writer);
		}
	}
	fclose(Writer);

	delete[] Buffer;
	return 0;
}

