#ifndef gubg_pattern_Observer
#define gubg_pattern_Observer

#include <set>

#define GUBG_MODULE_ "Observable"
#include "gubg/log/begin.hpp"
namespace gubg { namespace pattern {

		template <typename Message>
			class Observer_itf
			{
				public:
					virtual void notify(Message) = 0;
			};

		template <typename Outer, typename Message>
			class Observer_ftop;

		template <typename Message>
			class Observable
			{
				public:
					template <typename Outer>
						bool addObserver(Observer_ftop<Outer, Message> &obs)
						{
							S();
							if (obs.observable_)
							{
								L("Currently, only one observable per observer is allowed");
								return false;
							}
							LockGuard lg(observers_mutex_);
							obs.observable_ = this;
							observers_.insert(&obs);
							return true;
						}

					template <typename Outer>
					bool removeObserver(Observer_ftop<Outer, Message> &obs)
					{
						S();
						if (!obs.observable_)
						{
							L("No observable was registered");
							return false;
						}
						if (obs.observable_ != this)
						{
							L("This observer was not registered to us");
							return false;
						}
						LockGuard lg(observers_mutex_);
						obs.observable_ = 0;
						observers_.erase(&obs);
						return true;
					}

					void notifyObservers(Message msg)
					{
						S();
						LockGuard lg(observers_mutex_);
						//We copy the list of observers to make sure it is stable (an observer might register a new one on its notify)
						//We still do all notification under mutex, else, observers might disappear and we will call a dangling pointer
						//Make sure you place the observer ftop member at the end to make sure it will unregister itself before the Observer
						//object is too destoyed to be able to handle the notify call
						Observers observers = observers_;
						for (auto &obs: observers)
							obs->notify(msg);
					}

				private:
					typedef Observer_itf<Message> *ObserverT;
					typedef std::set<ObserverT> Observers;
					Observers observers_;
					typedef std::recursive_mutex Mutex;
					typedef std::lock_guard<Mutex> LockGuard;
					Mutex observers_mutex_;
			};

} }
#include "gubg/log/end.hpp"

#define GUBG_MODULE_ "Observer"
#include "gubg/log/begin.hpp"
namespace gubg { namespace pattern {

	//Place an instance of this ftop in your Observer class. Make sure it is near the end of the declaration to make sure it will
	//unregister itself soon enough upon destruction. Using inheritance here won't work well here since the parents are destoyed after all
	//members, so the chance is very low that a notify() call during destruction will be handled correctly
	template <typename Outer, typename Message>
		class Observer_ftop: public Observer_itf<Message>
	{
		public:
			Observer_ftop(Outer &outer): outer_(outer) {}
			~Observer_ftop()
			{
				if (observable_)
					observable_->removeObserver(*this);
			}

			virtual void notify(Message msg)
			{
				outer_.notify(msg);
			}

		private:
			Outer &outer_;
			friend class Observable<Message>;
			Observable<Message> *observable_ = 0;
	};

} }
#include "gubg/log/end.hpp"

#endif
