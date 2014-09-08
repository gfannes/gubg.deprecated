#include "fff/Linker.hpp"
#include "fff/Board.hpp"
#include "fff/Execute.hpp"
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
		for (auto tv: tvs)
		{
			if (false) {}
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
			}
		}

		ostringstream oss;
		oss << "g++ -std=c++0x -o " << executable;
		for (auto obj: objects)
			oss << " " << obj;
		L("Linking " << executable);
		MSS(execute(oss.str()));
		board.add(Tag("c++", "executable"), executable);

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
