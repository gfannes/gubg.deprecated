#ifndef gubg_pointer_StackPointer_hpp
#define gubg_pointer_StackPointer_hpp

namespace
{
	template <typename T>
		class StackPointer
		{
			public:
				StackPointer():
					valid(false){}
				~StackPointer()
				{
					if (valid)
						reinterpret_cast<T*>(buffer)->~T();
				}
				template <typename... Args>
					bool create(Args&&... args)
					{
						if (valid)
							return false;
						new (buffer) T(std::forward<Args>(args)...);
						valid = true;
						return true;
					}
				template <typename... Args>
					T &operator()(Args&&... args)
					{
						create(std::forward<Args>(args)...);
						return *reinterpret_cast<T*>(buffer);
					}
			private:
				size_t buffer[(sizeof(T)+sizeof(size_t)-1)/sizeof(size_t)];
				bool valid;
		};
}

#endif
