#ifndef HEADER_rinle_model_Model_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Model_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "rinle/model/FileInfo.hpp"

#define GUBG_MODULE_ "Model"
#include "gubg/log/begin.hpp"
namespace rinle
{
    namespace model
    {
        class Model
        {
            public:
                bool getLines(Lines &lines, const File file, size_t startLine, size_t nrLines) const
                {
                    S();L(STREAM(file, startLine, nrLines));
                    assert(invariants_());
                    auto it = infoPerFile_.find(file);
                    if (it == infoPerFile_.end())
                        return false;
                    const auto &info = *it->second;
                    return info.getLines(lines, startLine, nrLines);
                }

                FileInfo::Ptr getCurrent() const { return current_; }

                void proceed(int nrSteps)
                {
                    if (!current_)
                        return;
                    current_->proceed(nrSteps);
                }

                void load(const File &file)
                {
                    assert(invariants_());
                    auto &info = infoPerFile_[file];
                    if (!info)
                        info = FileInfo::create(file);
                    current_ = info;
                    assert(invariants_());
                }

            private:
                bool invariants_() const
                {
                    for (auto info: infoPerFile_)
                        if (!info.second)
                            //We require non-null info pointers
                            return false;
                    return true;
                }
                typedef std::map<File, FileInfo::Ptr> InfoPerFile;
                InfoPerFile infoPerFile_;
                FileInfo::Ptr current_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif