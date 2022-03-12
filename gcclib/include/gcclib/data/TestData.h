#pragma once

#include <gcclib/PipeTransfer.h>
#include <sstream>
#include <string>
class TestData :
    public PipeSerializedObject
{
public:

    TestData();
    void Fill();

    // Inherited via PipeSerializedObject
    virtual void Write(PipeTransfer* transfer) override;
    virtual void Read(PipeTransfer* transfer) override;

    operator std::string() const
    {
        std::stringstream ss;
        ss << "String: " << testString << "; Vector[0]: " << testVector[0] << "; Int: " << testInt;
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& Str, TestData const& v) {
        return Str << std::string(v);
    }

private:
    std::string testString;
    std::vector<byte> testVector;
    int32_t testInt;
};

