#ifndef HEADER_fff_agents_Chai_hpp_ALREADY_INCLUDED
#define HEADER_fff_agents_Chai_hpp_ALREADY_INCLUDED

#include "fff/Agent.hpp"
#include "gubg/file/File.hpp"
#include "gubg/chai/Engine.hpp"
#include <functional>
#include <queue>

namespace fff { namespace agents { 

    class Chai: public Agent_itf
    {
        public:
            Chai(const gubg::file::File &script);

			std::string name() const override;
			ReturnCode process(Board &) override;

            bool ok() const {return (bool)engine_;}

        private:
            Chai(const Chai &) = delete;
            Chai &operator=(const Chai &) = delete;

            size_t ix_ = 0;
            std::string name_;
            gubg::chai::Engine_ptr engine_;
            typedef std::function<void (const std::string &tag, const std::string &value, int)> Callback;
            Callback callback_;
            bool cb_was_set_ = false;
            void add_callback_(const std::string &name, const Callback &);

            typedef std::queue<std::pair<std::string, std::string>> Items;
            Items items_;
            void add_item_(const std::string &tag, const std::string &value);
    };

} } 

#endif
