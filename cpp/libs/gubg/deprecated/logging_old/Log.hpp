#ifndef HEADER_gubg_deprecated_logging_old_Log_hpp_ALREADY_INCLUDED
#define HEADER_gubg_deprecated_logging_old_Log_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <sstream>

#define LOG_S(msg) \
    gubg::Log::addIfEmpty(gubg::Log::Tree, gubg::Log::COut); \
    std::ostringstream local_gubg_logging_scope_message; \
    local_gubg_logging_scope_message << msg; \
    gubgLogScope(local_gubg_logging_scope_message.str())
#define LOG_M(msg) \
    { \
        gubg::Log::addIfEmpty(gubg::Log::Tree, gubg::Log::COut); \
        std::ostringstream local_gubg_logging_message; \
        local_gubg_logging_message << msg; \
        gubgLog(local_gubg_logging_message.str()); \
    } while (false)

#define gubgLogScope(message) gubg::Log::Scope gubg_Log_Scope(__FILE__, __LINE__, message)
#define gubgLog(message) gubg_Log_Scope(__FILE__, __LINE__, message)

namespace gubg
{
    class Log
    {
    public:
        class Output
        {
        public:
            class Primitive
            {
            public:
                virtual ~Primitive(){};

                virtual void write(const std::string &str) = 0;
            };

            Output(Primitive *primitive);
            virtual ~Output();

            virtual void newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag) = 0;
            virtual void closeLevel(const std::string &tag) = 0;
            virtual void newLine(const std::string &fileName, unsigned int lineNr) = 0;
            virtual void closeLine() = 0;
            virtual Output &operator<<(const std::string &str) = 0;

            Primitive *swapPrimitive(Primitive *primitive);

        protected:
            void write(const std::string &str);

        private:
            // Disallowed
            Output();
            Output(const Output &output){};
            Output &operator=(const Output &output){};

            Primitive *_primitive;
        };

        class Scope
        {
        public:
            Scope(const std::string &fileName, unsigned int lineNr, const std::string &msg);
            ~Scope();
            Scope &operator()(const std::string &fileName, unsigned int lineNr, const std::string &msg);
            Scope &operator<<(const std::string &msg);
            Scope &operator<<(int value);
            Scope &operator<<(unsigned int value);
            Scope &operator<<(void *ptr);

        private:
            std::string _msg;
            bool _lineIsOpen;
        };

        virtual ~Log();

        static Log &instance();

        enum LogFormat
        {
            XML,
            Tree,
        };
        enum LogSink
        {
            COut,
            File,
        };
        static void add(LogFormat logFormat, LogSink logSink, const std::string &fileName = "");
        static void addIfEmpty(LogFormat logFormat, LogSink logSink, const std::string &fileName = "");

        void newLevel(const std::string &fileName, unsigned int lineNr, const std::string &tag);
        void closeLevel(const std::string &tag);
        void newLine(const std::string &fileName, unsigned int lineNr);
        void closeLine();
        Log &operator<<(const std::string &str);

    private:
        // Singleton: disallow constructor
        Log();
        // Disallow copying
        Log(const Log& log);
        Log &operator=(const Log& log);

        std::vector<Output*> _outputs;
    };
};

#endif
