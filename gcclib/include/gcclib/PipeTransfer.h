#pragma once
#include <Windows.h>

#include <string>
#include <cstddef>
#include <vector>

typedef unsigned char byte;

class PipeTransfer;
class PipeSerializedObject
{
public:
	virtual void Write(PipeTransfer* transfer) = 0;
	virtual void Read(PipeTransfer* transfer) = 0;
};

class PipeTransfer
{
public:
	PipeTransfer(const std::string& name);

	bool Create();
	bool Connect();
	bool WaitForConnection();
	bool IsPipeOpened();

	void ReadBytes(void* buffer, size_t size);
	void WriteBytes(void* buffer, size_t size);
	
	std::vector<byte> ReadVector();
	void WriteVector(std::vector<byte>& vector);

	std::string ReadString();
	void WriteString(std::string& value);

	void ReadObject(PipeSerializedObject& object);
	void WriteObject(PipeSerializedObject& object);

	template<class T>
	T Read() 
	{
		T val = {};
		ReadBytes(&val, sizeof(T));
		return val;
	}

	template<class T>
	void Write(T val) 
	{
		WriteBytes(&val, sizeof(T));
	}

private:

	std::string name;
	HANDLE hPipe;

};

