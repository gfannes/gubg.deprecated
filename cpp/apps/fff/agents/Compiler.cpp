#include "fff/agents/Compiler.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "gubg/parallel/ForEach.hpp"
#include <list>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
    RecursiveDependencies collectAndHashDependencies_(const TagValue &tv, const Board &board)
    {
        //The include parser generates c++.include TVs, the resolves translates them into c++.source or c++.header
        //We only take the headers, the corresponding c++.source does not influence whoever is using the header
        Tags excludes; excludes.insert(Tag{"c++.source"}); excludes.insert(Tag{"c.source"});
        return board.getRecursiveDependencies(tv, excludes);
    }
    Compiler::Compiler()
        : compiler_(compiler::Vendor::GCC)
    {
    }
    ReturnCode Compiler::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        if (addHashTags_())
        {
            board.addItem(Tag("hash.tag"), Value("c++.source"));
            board.addItem(Tag("hash.tag"), Value("c.source"));
            board.addItem(Tag("hash.tag"), Value("c++.header"));
            board.addItem(Tag("hash.tag"), Value("c.header"));
            MSS_RETURN_OK();
        }

        auto tvs = board.getFrom(ix_);
        ix_ += tvs.size();

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == Tag("start") && tv.value.as_string() == "gcc")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::GCC);
            }
            else if (tv.tag == Tag("start") && tv.value.as_string() == "cl")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::MSC);
            }
            else if (tv.tag == Tag("start") && tv.value.as_string() == "clang")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::CLang);
            }
            else if (tv.tag == Tag("c++.flag"))
            {
                compiler_.addOption(tv.value.as_string());
            }
            else if (tv.tag == Tag("c++.define"))
            {
                compiler_.addDefine(tv.value.as_string());
            }
            else if (tv.tag == Tag("c++.include_path"))
            {
                compiler_.addIncludePath(tv.value.as_file());
            }
            else if (tv.tag == Tag("c++.force_include"))
            {
                compiler_.addForceInclude(tv.value.as_file());
            }
            else if (tv.tag == Tag("start") && tv.value.as_string() == "release")
            {
                compiler_.addOption("release");
                build_type_was_set_ = true;
            }
            else if (tv.tag == Tag("start") && tv.value.as_string() == "debug")
            {
                compiler_.addOption("debug");
                build_type_was_set_ = true;
            }
            else if (tv.tag == Tag("start") && tv.value.as_string() == "shared")
            {
                compiler_.addOption("shared");
            }
        }
        if (!build_type_was_set_)
        {
            compiler_.addOption("debug");
            build_type_was_set_ = true;
        }

        CreateMgr create_mgr;
        typedef list<CreateJob> Jobs;
        Jobs jobs;
        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == Tag("cache"))
            {
                create_mgr.setCache(tv.value.as_file());
            }
            else if (tv.tag == Tag("c++.source"))
            {
                const auto source = tv.value.as_file();
                SS(source);
                file::File obj = source;
                {
                    obj.setExtension("cpp.obj");
                    //Make the object file hidden. We place is next to the original cpp file,
                    //else, we have to create a directory somewhere
                    string basename;
                    if (obj.popBasename(basename))
                        obj << (string(".")+basename);
                }
                string cmd;
                compiler_.compile(cmd, source, obj, compiler::Language::Cpp);
                CreateJob job;
                job.file = obj;
                job.command = cmd;
                job.dependencies = collectAndHashDependencies_(tv, board);
                jobs.push_back(job);
                const Tag tag("c++.object");
                board.addItem(tag, obj);
            }
            else if (tv.tag == Tag("c.source"))
            {
                const auto source = tv.value.as_file();
                SS(source);
                file::File obj = source;
                {
                    obj.setExtension("c.obj");
                    //Make the object file hidden. We place is next to the original cpp file,
                    //else, we have to create a directory somewhere
                    string basename;
                    if (obj.popBasename(basename))
                        obj << (string(".")+basename);
                }
                string cmd;
                compiler_.compile(cmd, source, obj, compiler::Language::C);
                CreateJob job;
                job.file = obj;
                job.command = cmd;
                job.dependencies = collectAndHashDependencies_(tv, board);
                jobs.push_back(job);
                const Tag tag("c.object");
                board.addItem(tag, obj);
            }
        }

        {
            list<ReturnCode> errors;
            mutex m;
            auto ftor = [&](const CreateJob &job){
                //Check for previous errors, if so, we stop already
                {
                    lock_guard<mutex> l(m);
                    if (!errors.empty())
                        return;
                }
                //Execute the job and process its return code
                switch (const auto rc = create_mgr.create(job))
                {
                    case ReturnCode::OK: break;
                    default:
                                         {
                                             lock_guard<mutex> l(m);
                                             errors.push_back(rc);
                                         }
                                         break;
                }
            };
            gubg::parallel::for_each(jobs.begin(), jobs.end(), ftor, 0);
            MSS(errors.empty(), CompileFailure);
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
