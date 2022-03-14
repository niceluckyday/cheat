#include <pch.h>

#include <gcclib/data/TestData.h>

TestData::TestData() : testString({}), testVector({}), testInt(0) { }

void TestData::Fill() 
{
	testString = "TEST TEST";
	testVector = { 5, 10, 15, 25, 35, 40 };
	testInt = 123456;
}

void TestData::Write(PipeTransfer* transfer)
{
	transfer->WriteString(testString);
	transfer->WriteVector(testVector);
	transfer->Write(testInt);
}

void TestData::Read(PipeTransfer* transfer)
{
	testString = transfer->ReadString();
	testVector = transfer->ReadVector();
	testInt = transfer->Read<int32_t>();
}
