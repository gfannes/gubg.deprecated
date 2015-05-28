#include "fff/agents/Compiler.hpp"
#include "fff/Create.hpp"
#include "fff/Tags.hpp"
#include "gubg/bbs/Board.hpp"
#include "gubg/parallel/ForEach.hpp"
#include <list>
using namespace gubg;
using namespace gubg::bbs;
using namespace std;

#define GUBG_MODULE_ "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { namespace agents { 
    RecursiveDependencies collectAndHashDependencies_(const Item &tv, const Board &board)
    {
        //The include parser generates cpp_include TVs, the resolves translates them into cpp_source or cpp_header
        //We only take the headers, the corresponding cpp_source does not influence whoever is using the header
        Tags excludes; excludes.insert(cpp_source); excludes.insert(c_source);
        return board.getRecursiveDependencies(tv, excludes);
    }
    Compiler::Compiler()
        : compiler_(compiler::Vendor::GCC)
    {
    }
    gubg::bbs::ReturnCode Compiler::process(gubg::bbs::Board &board)
    {
        MSS_BEGIN(gubg::bbs::ReturnCode);

        if (addHashTags_())
        {
            board.addItem(hash_tag, cpp_source);
            board.addItem(hash_tag, c_source);
            board.addItem(hash_tag, cpp_header);
            board.addItem(hash_tag, c_header);
            MSS_RETURN_OK();
        }

        auto tvs = board.getFrom(ix_);
        ix_ += tvs.size();

        for (auto tv: tvs)
        {
            if (false) {}
            else if (tv.tag == start && tv.value == "gcc")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::GCC);
            }
            else if (tv.tag == start && tv.value == "cl")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::MSC);
            }
            else if (tv.tag == start && tv.value == "clang")
            {
                if (is_default_compiler_())
                    compiler_ = fff::Compiler(compiler::Vendor::CLang);
            }
            else if (tv.tag == cpp_flag)
            {
                compiler_.addOption(tv.value);
            }
            else if (tv.tag == cpp_define)
            {
                compiler_.addDefine(tv.value);
            }
            else if (tv.tag == cpp_include_path)
            {
                compiler_.addIncludePath(tv.value);
            }
            else if (tv.tag == cpp_force_include)
            {
                compiler_.addForceInclude(tv.value);
            }
            else if (tv.tag == start && tv.value == "release")
            {
                compiler_.addOption("release");
                build_type_was_set_ = true;
            }
            else if (tv.tag == start && tv.value == "debug")
            {
                compiler_.addOption("debug");
                build_type_was_set_ = true;
            }
            else if (tv.tag == start && tv.value == "shared")
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
            else if (tv.tag == cache_dir)
            {
                create_mgr.setCache(tv.value);
            }
            else if (tv.tag == cpp_source)
            {
                const file::File source = tv.value;
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
                board.addItem(cpp_object, obj);
            }
            else if (tv.tag == c_source)
            {
                const file::File source = tv.value;
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
                board.addItem(c_object, obj);
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
            MSS(errors.empty());//CompileFailure
        }

        MSS_END();
    }
} } 
#include "gubg/log/end.hpp"
