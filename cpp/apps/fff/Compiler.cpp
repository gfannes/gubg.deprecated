#include "fff/Compiler.hpp"
#include "fff/Board.hpp"
#include "fff/Create.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Compiler"
#include "gubg/log/begin.hpp"
namespace fff { 
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

        CreateMgr create_mgr(file::File("/tmp"));
		for (auto tv: tvs)
		{
			if (false) {}
			else if (tv.first == Tag("c++", "source"))
			{
				const auto source = tv.second.file();
				file::File obj = source; obj.setExtension("obj");
				ostringstream oss;
				oss << "g++ -std=c++0x -c " << source << " -o " << obj << " " << options;
				L("Compiling " << source << " into " << obj);
                CreateJob job;
                job.files.insert(obj);
                job.command = oss.str();
				MSS(create_mgr.create(job));
				board.add(Tag("c++", "object"), obj);
			}
		}

		MSS_END();
	}
} 
#include "gubg/log/end.hpp"
