#include "fff/Linker.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "gubg/file/Filesystem.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Linker"
#include "gubg/log/begin.hpp"
namespace fff { 
	ReturnCode Linker::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		MSS_Q(run_());
		auto tvs = board.getFrom(0);

#if 0
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
#endif

		file::File executable;
		OnlyOnce setExecutable;
		vector<file::File> objects;
        CreateMgr create_mgr;
		Dependencies dependencies;
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("cache"))
			{
				create_mgr.setCache(tv.second.file());
			}
			else if (tv.first == Tag("c++", "source"))
			{
				if (setExecutable())
				{
					executable = tv.second.file();
					executable.setExtension("tmp.exe");
				}
			}
			else if (tv.first == Tag("c++", "object"))
			{
				objects.push_back(tv.second.file());
				dependencies.insert(tv);
			}
			else if (tv.first == Tag("c", "object"))
			{
				objects.push_back(tv.second.file());
				dependencies.insert(tv);
			}
		}

		ostringstream oss;
		oss << "g++ -std=c++0x -o " << executable;
		for (auto obj: objects)
			oss << " " << obj;
        CreateJob job;
		job.files.insert(executable);
        job.command = oss.str();
		job.dependencies = board.hash(dependencies);
        MSS(create_mgr.create(job));
		board.add(Tag("c++", "executable"), executable);

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
