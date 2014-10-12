#include "fff/Linker.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Linker"
#include "gubg/log/begin.hpp"
namespace fff { 

	enum ExeType {Exe, Shared};

	ReturnCode Linker::process(Board &board)
	{
		MSS_BEGIN(ReturnCode);

		if (!run_())
			MSS_RETURN_OK();

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
		vector<string> libs;
		vector<file::File> libPaths;
		vector<string> options;
        CreateMgr create_mgr;
		Dependencies dependencies;
		ExeType exeType = Exe;
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("start"))
			{
				if (tv.second.string() == "shared")
					exeType = Shared;
			}
			else if (tv.first == Tag("cache"))
			{
				create_mgr.setCache(tv.second.file());
			}
			else if (tv.first == Tag("c++", "source"))
			{
				if (setExecutable())
					executable = tv.second.file();
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
			else if (tv.first == Tag("c++", "include"))
            {
#ifdef GUBG_LINUX
                if (tv.second.string() == "dlfcn.h")
                    libs.push_back("dl");
#endif
#ifdef GUBG_POSIX
                if (tv.second.string() == "thread")
                    options.push_back("pthread");
#endif
            }
			else if (tv.first == Tag("c++", "library_path"))
            {
				libPaths.push_back(tv.second.file());
            }
			else if (tv.first == Tag("c++", "library"))
            {
				libs.push_back(tv.second.string());
            }
		}

		ostringstream oss;
		oss << "g++ -std=c++0x ";

		switch (exeType)
		{
			case Exe:
				executable.setExtension("exe");
				break;
			case Shared:
				executable.setExtension("dll");
				oss << "-shared ";
				break;
		}
		oss << "-o " << executable;

		for (auto obj: objects)
			oss << " " << obj;

		for (auto opt: options)
			oss << " -" << options;

		for (auto lp: libPaths)
			oss << " -L" << lp;

		for (auto lib: libs)
			oss << " -l" << lib;

        CreateJob job;
		job.files.insert(executable);
        job.command = oss.str();
		job.dependencies = board.hash(dependencies);
        MSS(create_mgr.create(job), LinkFailure);
        {
            using namespace gubg::file;
            MSS(file::chmod(executable, {All, Read, Read}));
        }

		switch (exeType)
		{
			case Exe:    board.add(Tag("c++", "executable"), executable); break;
			case Shared: board.add(Tag("c++", "shared_object"), executable); break;
		}
		
		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
