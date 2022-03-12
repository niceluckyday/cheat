#include <pch.h>
#include <gcclib/PipeTransfer.h>

#include <iostream>
#include <sstream>

#include <gcclib/util.h>

PipeTransfer::PipeTransfer(const std::string& name) 
{
	std::stringstream ss;
	ss << "\\\\.\\pipe\\" << name;
	this->name = ss.str();
	this->hPipe = 0;
}

bool PipeTransfer::Create()
{
	if (hPipe)
		CloseHandle(hPipe);
	hPipe = CreateNamedPipe(name.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 256 * 1024, 16, INFINITE, NULL);
	return IsPipeOpened();
}

bool PipeTransfer::IsPipeOpened()
{
	return hPipe && hPipe != INVALID_HANDLE_VALUE;
}

bool PipeTransfer::Connect()
{
	if (IsPipeOpened())
		CloseHandle(hPipe);

	hPipe = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	return IsPipeOpened();
}

bool PipeTransfer::WaitForConnection()
{
	return ConnectNamedPipe(hPipe, nullptr);
}

void PipeTransfer::ReadBytes(void* buffer, size_t size)
{
	if (size == 0 || !IsPipeOpened()) return;
	
	DWORD readCount = 0;
	auto result = ReadFile(hPipe, buffer, size, &readCount, nullptr);
	if (!result || readCount < size)
	{
		LogLastError("Failed read from pipe.");
		CloseHandle(hPipe);
		hPipe = 0;
	}
}

void PipeTransfer::WriteBytes(void* buffer, size_t size)
{
	if (size == 0 || !IsPipeOpened()) return;

	DWORD writenCount = 0;
	auto result = WriteFile(hPipe, buffer, size, &writenCount, nullptr);
	if (!result || writenCount < size)
	{
		LogLastError("Failed write to pipe.");
		CloseHandle(hPipe);
		hPipe = 0;
	}
}

std::string PipeTransfer::ReadString()
{
	auto size = Read<int32_t>();
	std::string result((const size_t)size, '\0');
	ReadBytes(result.data(), size);
	return result;
}

void PipeTransfer::WriteString(std::string& value)
{
	Write<int32_t>(value.length());
	WriteBytes(value.data(), value.length());
}

void PipeTransfer::ReadObject(PipeSerializedObject& object)
{
	object.Read(this);
}

void PipeTransfer::WriteObject(PipeSerializedObject& object)
{
	object.Write(this);
}

std::vector<byte> PipeTransfer::ReadVector()
{
	auto size = Read<int32_t>();
	std::vector<byte> result((const size_t)size, (byte)0);
	ReadBytes(result.data(), size);
	return result;
}

void PipeTransfer::WriteVector(std::vector<byte>& value)
{
	Write<int32_t>(value.size());
	WriteBytes(value.data(), value.size());
}
