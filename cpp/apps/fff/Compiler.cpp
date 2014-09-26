#include "fff/Compiler.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "gubg/parallel/ForEach.hpp"
#include <list>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { 
	Hash collectAndHashDependencies_(const TagValue &tv, const Board &board)
	{
		Dependencies dependencies;
		typedef std::list<TagValue> Stage;
		Stage stage; stage.push_back(tv);
		while (!stage.empty())
		{
			TagValue tv = stage.front(); stage.pop_front();
			if (tv.first == Tag("c++", "source") || tv.first == Tag("c++", "header"))
				//Only headers and the source file should be taken into account
				dependencies.insert(tv);
			auto deps = board.getDependencies(tv);
			for (auto d: deps)
			{
				//The include parser generates c++.include TVs, the resolves translates them into c++.source or c++.header
				//We only take the headers, the corresponding c++.source does not influence whoever is using the header
				if (d.first == Tag("c++", "include") || d.first == Tag("c++", "header"))
				{
					if (!dependencies.count(d))
						stage.push_back(d);
				}
			}
		}
		return board.hash(dependencies);
	}
	ReturnCode Compiler::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		auto tvs = board.getFrom(ix_);
		ix_ += tvs.size();

		vector<file::File> includePaths;
		vector<string> options;
		set<string> defines;
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("c++", "include_path"))
				includePaths.push_back(tv.second.file());
			else if (tv.first == Tag("start") && tv.second.string() == "release")
            {
                options.push_back("O3");
                defines.insert("GUBG_RELEASE");
            }
			else if (tv.first == Tag("start"))
			{
				if (tv.second.string() == "shared")
					options.push_back("fPIC");
			}
		}
        if (defines.count("GUBG_RELEASE") == 0)
        {
            options.push_back("g");
            defines.insert("GUBG_DEBUG");
        }

		string options_str;
		{
			ostringstream oss;
			for (auto o: options)
				oss << "-" << o << " ";
			for (auto ip: includePaths)
				oss << "-I" << ip << " ";
			for (auto d: defines)
				oss << "-D" << d << " ";
			options_str = oss.str();
		}

		CreateMgr create_mgr;
		typedef list<CreateJob> Jobs;
		Jobs jobs;
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("cache"))
			{
				create_mgr.setCache(tv.second.file());
			}
			else if (tv.first == Tag("c++", "source"))
			{
				const auto source = tv.second.file();
				SS(source);
				file::File obj = source; obj.setExtension("cpp.obj");
				ostringstream oss;
				oss << "g++ -std=c++0x -c " << source << " -o " << obj << " " << options_str;
                CreateJob job;
                job.files.insert(obj);
                job.command = oss.str();
				job.dependencies = collectAndHashDependencies_(tv, board);
				jobs.push_back(job);
				board.add(Tag("c++", "object"), obj);
			}
			else if (tv.first == Tag("c", "source"))
			{
				const auto source = tv.second.file();
				SS(source);
				file::File obj = source; obj.setExtension("c.obj");
				ostringstream oss;
				oss << "gcc -c " << source << " -o " << obj << " " << options_str;
                CreateJob job;
                job.files.insert(obj);
                job.command = oss.str();
				job.dependencies = collectAndHashDependencies_(tv, board);
				jobs.push_back(job);
				board.add(Tag("c", "object"), obj);
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
} 
#include "gubg/log/end.hpp"
