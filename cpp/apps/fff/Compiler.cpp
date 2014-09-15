#include "fff/Compiler.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
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

		MSS_Q(run_());
		auto tvs = board.getFrom(0);

		vector<file::File> includePaths;
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("c++", "include_path"))
				includePaths.push_back(tv.second.file());
		}
		string options;
		{
			ostringstream oss;
			for (auto ip: includePaths)
				oss << "-I" << ip << " ";
			options = oss.str();
		}

        CreateMgr create_mgr;
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
				file::File obj = source; obj.setExtension("obj");
				ostringstream oss;
				oss << "g++ -std=c++0x -c " << source << " -o " << obj << " " << options;
                CreateJob job;
                job.files.insert(obj);
                job.command = oss.str();
				job.dependencies = collectAndHashDependencies_(tv, board);
				MSS(create_mgr.create(job));
				board.add(Tag("c++", "object"), obj);
			}
			else if (tv.first == Tag("c", "source"))
			{
				const auto source = tv.second.file();
				SS(source);
				file::File obj = source; obj.setExtension("obj");
				ostringstream oss;
				oss << "gcc -c " << source << " -o " << obj << " " << options;
                CreateJob job;
                job.files.insert(obj);
                job.command = oss.str();
				job.dependencies = collectAndHashDependencies_(tv, board);
				MSS(create_mgr.create(job));
				board.add(Tag("c", "object"), obj);
			}
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
