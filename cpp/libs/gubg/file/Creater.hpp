#ifndef HEADER_gubg_file_Creater_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Creater_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/hash/MD5.hpp"
#include <sstream>

#define GUBG_MODULE "Creater"
#include "gubg/log/begin.hpp"
namespace gubg { namespace file {

	template <typename Receiver>
		class Creater_crtp
		{
			public:
				typedef std::vector<File> Files;

				ReturnCode setCache(File dir)
				{
					MSS_BEGIN(ReturnCode);
					Cache cache(new File(dir));
					MSS(determineType(*cache));
					MSS(cache->type() == File::Directory);
					cache_ = std::move(cache);
					MSS_END();
				}

				template <typename SufficientInfo>
					ReturnCode operator()(const Files &files, const SufficientInfo &si) const
					{
						MSS_BEGIN(ReturnCode);
						MSS(remove_(files));
						Files cachedFiles;
						if (cache_)
						{
							std::ostringstream oss;
							oss << si;
							using namespace hash::md5;
							Stream md5_base;
							md5_base << oss.str();
							for (auto f: files)
							{
								Stream md5(md5_base);
								md5 << f.name();
								File cachedFile(*cache_);
								cachedFile << md5.hash_hex();
								cachedFile.setType(File::Unknown);
								cachedFiles.push_back(cachedFile);
							}
							bool foundAll = true;
							for (auto cf: cachedFiles)
							{
								foundAll = false;
								if (!isRegular(cf))
									continue;
								foundAll = true;
							}
							if (foundAll)
							{
								//We found all files in the cache, just copy them and finished
								size_t i = 0;
								for (auto &cf: cachedFiles)
								{
									receiver_().creater_copyFromCache(cf, files[i]);
									MSS(copy(cf, files[i]));
									++i;
								}
								MSS_END();
							}
						}
						//No cache present, or the files are not present in the cache => we have to generate them
						MSS(receiver_().creater_create(files, si));
						//Check that all files are present
						for (auto f: files)
							MSS(isRegular(f));
						//Put the generated files in the cache
						if (cache_)
						{
							size_t i = 0;
							for (auto &cf: cachedFiles)
							{
								receiver_().creater_copyToCache(files[i], cf);
								MSS(copy(files[i], cf));
								++i;
							}
						}
						MSS_END();
					}

				void creater_copyFromCache(const File &cached, const File &real) const {}
				void creater_copyToCache(const File &real, const File &cached) const {}

			private:
				typedef std::unique_ptr<File> Cache;

				Receiver &receiver_(){return static_cast<Receiver&>(*this);}
				const Receiver &receiver_() const {return static_cast<const Receiver&>(*this);}

				static ReturnCode remove_(const Files &files)
				{
					MSS_BEGIN(ReturnCode);
					for (auto f: files)
						remove(f);
					MSS_END();
				}

				Cache cache_;
		};

} }
#include "gubg/log/end.hpp"

#endif
