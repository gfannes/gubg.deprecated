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
    Hash collectAndHashDependencies_(const TagValue &tv, const Board &board)
    {
        Dependencies dependencies;
        typedef std::list<TagValue> Stage;
        Stage stage; stage.push_back(tv);
        while (!stage.empty())
        {
            TagValue tv = stage.front(); stage.pop_front();
            if (tv.tag == Tag("c++.source") || tv.tag == Tag("c++.header"))
                //Only headers and the source file should be taken into account
                dependencies.insert(tv);
            auto deps = board.getDependencies(tv);
            for (auto d: deps)
            {
                //The include parser generates c++.include TVs, the resolves translates them into c++.source or c++.header
                //We only take the headers, the corresponding c++.source does not influence whoever is using the header
                if (d.tag == Tag("c++.include") || d.tag == Tag("c++.header"))
                {
                    if (!dependencies.count(d))
                        stage.push_back(d);
                }
            }
        }
        return board.hash(dependencies);
    }
    Compiler::Compiler()
        : compiler_(compiler::Vendor::GCC)
    {
    }
    ReturnCode Compiler::process(Board &board)
    {
        MSS_BEGIN(ReturnCode);

        auto tvs = board.getFrom(ix_);
        ix_ += tvs.size();

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == Tag("start") && tv.value.as_string() == "cl")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::MSC);
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
                job.files.insert(obj);
                job.command = cmd;
                job.dependencies = collectAndHashDependencies_(tv, board);
                jobs.push_back(job);
                const Tag tag("c++.object");
                board.setTypeForTag(tag, Type::File);
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
                job.files.insert(obj);
                job.command = cmd;
                job.dependencies = collectAndHashDependencies_(tv, board);
                jobs.push_back(job);
                const Tag tag("c.object");
                board.setTypeForTag(tag, Type::File);
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
