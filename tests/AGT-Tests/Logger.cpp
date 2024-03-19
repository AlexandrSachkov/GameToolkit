#include "pch.h"

#include "AGT/log/DefaultLogger.h"
#include "AGT/log/DefaultLogFormatter.h"
#include "AGT/log/ILoggerSink.h"
#include "AGT/log/Log.h"
#include "AGT/log/LogEntryBuilder.h"

#include <array>
#include <filesystem>
#include <vector>
#include <memory>

TEST(Logger, EntryBuilder) {
    std::vector<char> data(100);
    
    {
        std::string str{ "This is a test" };

        AGT::LogEntryBuilder builder(data);
        builder.Write(str.c_str());
        EXPECT_EQ(str, std::string(data.data(), str.size()));
        EXPECT_EQ(str.size(), builder.GetSizeWritten());
    }

    {
        std::string str{ "This is a test" };

        AGT::LogEntryBuilder builder(data);
        builder.WriteLine(str.c_str());
        EXPECT_EQ(str + "\n", std::string(data.data()));
        EXPECT_EQ(str.size() + 1, builder.GetSizeWritten());
    }

    {
        std::string msg2Format{ "The values are num=%i str=%s" };
        int msg2Num = 25;
        std::string msg2Str{ "HELLO" };
        std::string msg2Expected{ "The values are num=" + std::to_string(msg2Num) + " str=" + msg2Str + "\n" };

        AGT::LogEntryBuilder builder(data);
        builder.WriteLine(msg2Format.c_str(), msg2Num, msg2Str.c_str());
        EXPECT_EQ(msg2Expected, std::string(data.data()));
        EXPECT_EQ(msg2Expected.size(), builder.GetSizeWritten());
    }

    {
        std::string longString{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
        "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
        "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
        };

        AGT::LogEntryBuilder builder(data);
        builder.WriteLine(longString.c_str());
        //max string will be 1 char smaller than the array as the last character is always the terminator
        EXPECT_EQ(std::string(longString.c_str(), data.size() - 2) + "\n", std::string(data.data()));
        EXPECT_EQ(data.size(), builder.GetSizeWritten());
    }

    {
        std::string line1 = "This is line 1.";
        std::string line2 = "And this is line 2.";
        std::string expected = line1 + "\n" + line2 + "\n";

        AGT::LogEntryBuilder builder(data);
        builder.WriteLine(line1.c_str());
        builder.WriteLine(line2.c_str());
        EXPECT_EQ(expected, std::string(data.data()));
        EXPECT_EQ(expected.size(), builder.GetSizeWritten());
    }

    {
        std::string str1 = "This is sentence #1.";
        std::string str2 = "And this is sentence #2.";
        std::string expected = str1 + str2 + "\n";

        AGT::LogEntryBuilder builder(data);
        builder.Write(str1.c_str());
        builder.Write(str2.c_str());
        builder.EndLine();

        EXPECT_EQ(expected, std::string(data.data()));
        EXPECT_EQ(expected.size(), builder.GetSizeWritten());
    }
}

TEST(Logger, DefaultLogFormatter) {
    std::vector<char> data(200);

    {
        std::filesystem::path filePath{ __FILE__ };
        const char* funcName = __func__;
        int lineNumber = __LINE__;
        std::string msgFormat{ "This is a test error: %i" };
        int pid = _getpid();
        size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        
        AGT::LogEntryBuilder builder(data);
        AGT::DefaultLogFormatter formatter;
        formatter.Format(builder, AGT::LogLevel::Error, filePath.string().c_str(), funcName, lineNumber, "This is a test error: %i", 12345);

        std::string result{ data.data() };
        EXPECT_TRUE(result.find(AGT::LogLevelToString(AGT::LogLevel::Error)) != std::string::npos);
        EXPECT_TRUE(result.find(filePath.filename().string().c_str()) != std::string::npos);
        EXPECT_TRUE(result.find(funcName) != std::string::npos);
        EXPECT_TRUE(result.find(std::to_string(lineNumber)) != std::string::npos);
        EXPECT_TRUE(result.find(std::to_string(pid)) != std::string::npos);
        EXPECT_TRUE(result.find(std::to_string(threadId)) != std::string::npos);
        EXPECT_TRUE(result.find("This is a test error: " + std::to_string(12345)) != std::string::npos);
        EXPECT_EQ(result.size(), builder.GetSizeWritten());
    }
}

struct MockSink : public AGT::ILoggerSink {
    void Write(const char* msg, size_t size) override {
        Message = msg;
        Size = size;
    };

    void Flush() override {

    };

    std::string Message;
    size_t Size{ 0 };
};

TEST(Logger, DefaultLogger) {
    const std::string msg{ "This is a test error: " };
    const int msgNum{ 12345 };
    const std::string expected{ msg + std::to_string(msgNum) };

    {
        auto sink1 = std::make_shared<MockSink>();
        auto sink2 = std::make_shared<MockSink>();

        std::array<std::shared_ptr<AGT::ILoggerSink>, 2> sinks = { sink1, sink2 };

        auto defaultLogger = std::shared_ptr<AGT::LoggerT>(AGT::LoggerT::Create(AGT::LogLevel::Debug, 1024, sinks));
        AGT::StaticHolder<AGT::LoggerT>::Set(defaultLogger);

        AGT_DEBUG((msg + "%i").c_str(), msgNum);

        EXPECT_TRUE(sink1->Message.find(expected) != std::string::npos);
        EXPECT_TRUE(sink1->Message.find(AGT::LogLevelToString(AGT::LogLevel::Debug)) != std::string::npos);

        EXPECT_TRUE(sink2->Message.find(expected) != std::string::npos);
        EXPECT_TRUE(sink2->Message.find(AGT::LogLevelToString(AGT::LogLevel::Debug)) != std::string::npos);
    }

    {
        auto sink1 = std::make_shared<MockSink>();
        auto sink2 = std::make_shared<MockSink>();

        std::array<std::shared_ptr<AGT::ILoggerSink>, 2> sinks = { sink1, sink2 };

        auto defaultLogger = std::shared_ptr<AGT::LoggerT>(AGT::LoggerT::Create(AGT::LogLevel::Warning, 1024, sinks));
        AGT::StaticHolder<AGT::LoggerT>::Set(defaultLogger);

        AGT_VERBOSE((msg + "%i").c_str(), msgNum);

        EXPECT_TRUE(sink1->Message.empty());
        EXPECT_TRUE(sink2->Message.empty());
    }
}