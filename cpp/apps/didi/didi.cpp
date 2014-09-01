#include "didi/Codes.hpp"
#include "didi/Options.hpp"
#include "didi/Finalize.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/Compare.hpp"
#include "gubg/hash/MD5.hpp"
#include <iostream>
#include <map>
#include <cstdlib>
#include <sstream>
using namespace didi;
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "didi"
#include "gubg/log/begin.hpp"
namespace  { 

	class FileMapper
	{
		public:
			typedef map<file::File, file::File> FileMap;

			FileMapper(file::File root): root_(root) {}

			FileMap getFileMap() const {return fileMap_;}

			ReturnCode recursor_discoveredFile(file::File f)
			{
				MSS_BEGIN(ReturnCode);
				MSS(file::determineType(f));
				switch (f.type())
				{
					case file::File::Directory:
						{
							const auto bn = f.basename();
							if (false) {}
							else if (bn == ".git") { MSS_QL(Skip); }
						}
						break;
					case file::File::Regular:
						if (skipExtension_(f.extension()))
							break;
						fileMap_[f.relative(root_)] = f;
						break;
				}
				MSS_END();
			}

		private:
			static bool skipExtension_(const std::string &ext)
			{
				if (ext == "o")
					return true;
				return false;
			}

			const file::File root_;
			FileMap fileMap_;
	};

	typedef gubg::hash::MD5::Hash Hash;
	Hash hash(const file::File &fn)
	{
		std::string content;
		file::read(content, fn);
		gubg::hash::MD5 md5;
		md5 << content;
		return md5.hash();
	}

	class Compare
	{
		public:
			template <typename Pair>
			void keycompare_both(const Pair &lhs, const Pair &rhs)
			{
				const auto &lfn = lhs.second;
				const auto &rfn = rhs.second;

				const auto lhash = hash(lfn);
				const auto rhash = hash(rfn);
				if (lhash == rhash)
					return;

				S();L("Diffing " << lfn << " and " << rfn);
				L(gubg::hash::MD5::to_hex(lhash));
				L(gubg::hash::MD5::to_hex(rhash));
				std::ostringstream command; command << "gvim -f -d " << lfn << " " << rfn;
				std::system(command.str().c_str());
			}

			template <typename Pair>
				void keycompare_lhs(const Pair &lhs) { }
			template <typename Pair>
				void keycompare_rhs(const Pair &rhs) { }
		private:
	};

    ReturnCode main_(int argc, char **argv)
    {
        MSS_BEGIN(ReturnCode);

		bool verbose = false;
        Options options;
        {
            OptionParser optionParser("Directory diff");
            optionParser.addSwitch("-h", "--help", "Displays this help", [&optionParser](){DIDI_FINALIZE_OK(optionParser.help());});
            optionParser.addSwitch("-v", "--verbose", "Verbose", [&verbose](){verbose = true;});

            auto args = OptionParser::createArgs(argc, argv);
            MSS(optionParser.parse(args));
			for (const auto &str: args)
				options.directories.push_back(gubg::file::File(str));
        }

        if (verbose)
            cout << options;

		if (options.directories.size() == 2)
		{
			typedef std::array<FileMapper::FileMap, 2> FileMaps;
			FileMaps fileMaps;
			for (auto it = fileMaps.begin(); it != fileMaps.end(); ++it)
			{
				const auto ix = (it - fileMaps.begin());
				const auto &directory = options.directories[ix];
				FileMapper fileMapper(directory);
				file::recurse(fileMapper, directory);
				*it = fileMapper.getFileMap();
				if (true)
					for (const auto &p: *it)
						L(p.first << " => " << p.second);
			}
			Compare compare;
			gubg::keycompare_maps(compare, fileMaps[0], fileMaps[1]);
		}

        MSS_END();
    }
} 
#include "gubg/log/end.hpp"

int main(int argc, char **argv)
{
    if (!gubg::mss::isOK(main_(argc, argv)))
        DIDI_FINALIZE_ERROR("Problems were encountered...");
    DIDI_FINALIZE_OK("Everything went OK");
    return 0;
}
