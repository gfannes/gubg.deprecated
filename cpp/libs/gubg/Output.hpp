#ifndef HEADER_gubg_Output_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Output_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <cassert>

#define GUBG_MODULE_ "Output"
#include "gubg/log/begin.hpp"
namespace gubg
{
	template <typename Decorator>
		class ScopedStream;

	class Stream
	{
		public:
			typedef std::shared_ptr<Stream> Ptr;
			static Ptr create(std::ostream &os) {Ptr ptr(new Stream); ptr->os_ = &os; return ptr;}
			static Ptr create(const gubg::file::File &fn) {Ptr ptr(new Stream); ptr->fn_ = fn; return ptr;}
			static Ptr null() {Ptr ptr(new Stream); return ptr;}

			int level() const {return level_;}
			void setLevel(int level) {level_ = level;}

			std::ostream &os()
			{
				while (!os_)
					createOS_();
				return *os_;
			}

		private:
			Stream(){}
			void createOS_()
			{
				S();
				if (!fn_.empty())
				{
					L("Creating real file \"" << fn_ << "\" for logging");
					of_.reset(new std::ofstream(fn_.name(), std::ios_base::out | std::ios_base::trunc));
				}
				else
				{
					L("Creating /dev/null for logging");
					of_.reset(new std::ofstream);
				}
				os_ = of_.get();
			}

			std::ostream *os_ = 0;
			int level_ = 0;
			gubg::file::File fn_;
			std::unique_ptr<std::ofstream> of_;

			template <typename Decorator>
				friend class ScopedStream;
			typedef std::recursive_mutex Mutex;
			typedef std::unique_lock<Mutex> UniqueGuard;
			mutable Mutex mutex_;
	};

	class StreamMgr
	{
		public:
			StreamMgr(): nullStream_(Stream::null()) {}
			static StreamMgr &instance() {static StreamMgr thiz; return thiz;}

			Stream::Ptr get_ptr(std::ostream &os)
			{
				LockGuard lock(mutex_);
				auto it = streamPerostream_.find(&os);
				if (it == streamPerostream_.end())
				{
					S();
					streamPerostream_[&os] = Stream::create(os);
					it = streamPerostream_.find(&os);
					assert(it != streamPerostream_.end());
					L("Created ostream " << it->second);
				}
				return it->second;
			}
			Stream &get(std::ostream &os)
			{
				return *get_ptr(os);
			}
			Stream &get(std::ostream &os, int level)
			{
				Stream &stream = get(os);
				if (level < stream.level())
					return *nullStream_;
				return stream;
			}
			Stream::Ptr get_ptr(const gubg::file::File &fn)
			{
				LockGuard lock(mutex_);
				auto it = streamPerFile_.find(fn);
				if (it == streamPerFile_.end())
				{
					S();
					streamPerFile_[fn] = Stream::create(fn);
					it = streamPerFile_.find(fn);
					assert(it != streamPerFile_.end());
					L("Created file stream " << it->second);
				}
				return it->second;
			}
			Stream &get(const gubg::file::File &fn)
			{
				return *get_ptr(fn);
			}
			Stream &get(const gubg::file::File &fn, int level)
			{
				Stream &stream = get(fn);
				if (level < stream.level())
					return *nullStream_;
				return stream;
			}

		private:
			typedef std::map<std::ostream*, Stream::Ptr> StreamPerostream;
			StreamPerostream streamPerostream_;
			typedef std::map<gubg::file::File, Stream::Ptr> StreamPerFile;
			StreamPerFile streamPerFile_;
			Stream::Ptr nullStream_;

			typedef std::mutex Mutex;
			typedef std::lock_guard<Mutex> LockGuard;
			mutable Mutex mutex_;
	};

	template <typename Decorator>
		class ScopedStream
		{
			public:
				ScopedStream(Stream &stream): stream_(stream), lock_(stream_.mutex_)
			{
				decorator_.before(stream_.os());
			}
				~ScopedStream()         
				{
					decorator_.after(stream_.os());
				}
				ScopedStream(ScopedStream &&ss): stream_(ss.stream_), lock_(std::move(ss.lock_)), decorator_(std::move(ss.decorator_)) { }

				template <typename T>
					ScopedStream &operator<<(const T &t)
					{
						stream_.os() << t;
						return *this;
					}

			private:
				ScopedStream(const ScopedStream &);
				ScopedStream &operator=(const ScopedStream &);

				Stream &stream_;
				Stream::UniqueGuard lock_;
				Decorator decorator_;
		};

	template <typename Decorator>
		ScopedStream<Decorator> output(Stream &stream)
		{
			return ScopedStream<Decorator>(stream);
		}

	template <typename Decorator>
		ScopedStream<Decorator> output(std::ostream &os, int level)
		{
			return ScopedStream<Decorator>(StreamMgr::instance().get(os, level));
		}
	template <typename Decorator>
		ScopedStream<Decorator> output(int level = 0)
		{
			return output<Decorator>(std::cout, level);
		}

	template <typename Decorator>
		ScopedStream<Decorator> output(const gubg::file::File &fn, int level)
		{
			return ScopedStream<Decorator>(StreamMgr::instance().get(fn, level));
		}
	template <typename Decorator>
		ScopedStream<Decorator> output(const std::string &fn, int level = 0)
		{
			return output<Decorator>(gubg::file::File(fn), level);
		}

	template <typename Decorator>
		class Output
		{
			public:
				Output(std::ostream &os): stream_(StreamMgr::instance().get_ptr(os)){}
				Output(const std::string &fn): stream_(StreamMgr::instance().get_ptr(fn)){}

				ScopedStream<Decorator> output()
				{
					return ScopedStream<Decorator>(*stream_);
				}
				ScopedStream<Decorator> output(int level)
				{
					if (level < stream_->level())
						return ScopedStream<Decorator>(*null_);
					return ScopedStream<Decorator>(*stream_);
				}
			private:
				Stream::Ptr stream_;
				Stream::Ptr null_ = Stream::null();
		};
}
#include "gubg/log/end.hpp"

#endif
