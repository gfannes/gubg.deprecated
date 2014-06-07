#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include "gubg/array.hpp"
#include "gubg/cassert.hpp"

#define GUBG_MODULE_ "Deserialize"
#include "gubg/log/begin.hpp"
namespace gubg { namespace msgpack {

	template <typename String>
		class Wrapper;

	//Dummy object
	template <typename String>
		class DummyObject 
		{
			public:
				void createObject(Wrapper<String> &, RoleId) {}
				void set(RoleId, Nil_tag){}
				void set(RoleId, long v){}
				void set(RoleId, bool v){}
				void set(RoleId, const String &str){}
				void createdObject(RoleId){}
		};

	//Wraps a reference to an object and a few methods (msgpack_set()), type-erasure style.
	//The non-virtual methods will call into those of the object passed during construction.
	template <typename String>
		class Wrapper
		{
			public:
				typedef Wrapper<String> Self;

				RoleId rid;

				Wrapper() {}
				//Type-erasure ctor, make sure t does not vanish
				template <typename T>
					explicit Wrapper(T &t)
					{
						//Type erasure: we will copy obj byte for byte into data_, including its vtable
						//Probably not very portable...
						Object<T> obj(t);
						const uint8_t *ptr = (uint8_t*)&obj;
						for (auto it = data_.begin(); it != data_.end(); ++it)
							*it = *ptr++;
						dataIsValid_ = true;
					}
				Wrapper(const Wrapper &rhs): rid(rhs.rid), data_(rhs.data_), dataIsValid_(rhs.dataIsValid_) {}
				Wrapper &operator=(const Wrapper &rhs)
				{
					rid = rhs.rid; data_ = rhs.data_; dataIsValid_ = rhs.dataIsValid_;
					return *this;
				}

				//Calls into the corresponding msgpack_createObject()/msgpack_set()/msgpack_createdObject() methods of the object passed during ctor.
				void createObject(Self &res, RoleId rid) {if (!dataIsValid_) return; itf_()->createObject(res, rid);}
				void set(RoleId rid, Nil_tag nil)        {if (!dataIsValid_) return; itf_()->set(rid, nil);}
				void set(RoleId rid, int v)              {if (!dataIsValid_) return; itf_()->set(rid, (long)v);}
				void set(RoleId rid, long v)             {if (!dataIsValid_) return; itf_()->set(rid, v);}
				void set(RoleId rid, bool v)             {if (!dataIsValid_) return; itf_()->set(rid, v);}
				void set(RoleId rid, const String &str)  {if (!dataIsValid_) return; itf_()->set(rid, str);}
				void createdObject(RoleId rid)           {if (!dataIsValid_) return; itf_()->createdObject(rid);}

			private:
				//Interface used in the deserialization framework. No need to implement this API, use wrap() to generate it.
				class Object_itf
				{
					public:
						virtual void createObject(Self&, RoleId) = 0;
						virtual void set(RoleId, Nil_tag) = 0;
						virtual void set(RoleId, long v) = 0;
						virtual void set(RoleId, bool v) = 0;
						virtual void set(RoleId, const String &str) = 0;
						virtual void createdObject(RoleId) = 0;
				};

				Object_itf *      itf_()       {assert(dataIsValid_); return       (Object_itf*)data_.data();}
				const Object_itf *itf_() const {assert(dataIsValid_); return (const Object_itf*)data_.data();}

				template <typename T>
					class Object: public Object_itf
				{
					public:
						Object(T &t): ptr_(&t) {}
						virtual void createObject(Self &res, RoleId rid) { obj_().msgpack_createObject(res, rid); };
						virtual void set(RoleId rid, Nil_tag nil)        { obj_().msgpack_set(rid, nil); }
						virtual void set(RoleId rid, long v)             { obj_().msgpack_set(rid, v); }
						virtual void set(RoleId rid, bool v)             { obj_().msgpack_set(rid, v); }
						virtual void set(RoleId rid, const String &str)  { obj_().msgpack_set(rid, str); }
						virtual void createdObject(RoleId rid)           { obj_().msgpack_createdObject(rid); };

					private:
						//Reversal of the type erasure
						T &obj_() {return *reinterpret_cast<T*>(ptr_);}
						//We will store the address of the wrapped object here
						void *ptr_;
				};

				//We will store a _binary_ copy of an Object instance into data_, with vtable and all
				typedef gubg::array<uint8_t, sizeof(Object<DummyObject<String>>)> Data;
				Data data_;
				bool dataIsValid_ = false;
		};

	//Helper funcions to wrap an object. Typically, you want to clear the object first,
	//reducing any dependency an previous garbage in it.
	template <typename String, typename T>
		Wrapper<String> wrap(T &t)
		{
			//We make sure to clear t before deserializing it
			t = T();
			return Wrapper<String>(t);
		}
	//If you know what you are doing, you can skip clearing the object
	template <typename String, typename T>
		Wrapper<String> wrapWithoutClear(T &t)
		{
			return Wrapper<String>(t);
		}

} }
#include "gubg/log/end.hpp"

#endif
