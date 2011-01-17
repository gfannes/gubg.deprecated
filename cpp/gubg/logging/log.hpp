#ifndef gubg_log_hpp
#define gubg_log_hpp

#include <string>
#include <vector>

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
