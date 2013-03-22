#ifndef HEADER_garf_Average_hpp_ALREADY_INCLUDED	
#define HEADER_garf_Average_hpp_ALREADY_INCLUDED

template<typename DataType, size_t Size>
class RobustAverage
{
public:
	RobustAverage():ageCounter_(1)
	{
	}
	size_t findOldestIndex()
	{
		oldestIndex = 0;
		size_t oldestAge = samples_[0].age_;
		for(size_t cnt = 1; cnt < Size; ++cnt)
		{
			DataType age = samples_[cnt].age;
			if(age > oldestAge)
			{
				oldestAge = age;
				oldestIndex = cnt;
			}
		}
		return oldestIndex;
	}
	void addSample(const DataType& sample)
	{
		size_t idx = findOldestIndex();
		Sample newSample(sample, ++ageCounter_);
		if(newSample < Samples_[idx])
		{
			// new sample is smaller, insert to the bottom
			while(idx >= 0)
			{
				if(newSample < samples_[idx - 1])
				{
					samples_[idx] = samples_[idx - 1];
				}
				else
				{
					samples_[idx] = newSample;
					break;
				}
				--idx;
			}
		}
	 	else
	 	{
	 		// new sample is larger, insert to the top
	 		while(idx < Size)
	 		{
				if(newSample > samples_[idx + 1])
				{
					samples_[idx] = samples_[idx + 1];
				}
				else
				{
					samples_[idx] = newSample;
					break;
				}
				++idx;
	 		}
	 	}
	}

private:
	struct Sample 
	{
		DataType value_;
		size_t age_;
		
		operator <(const Sample& rhs){value_ < rhs.value_;}
		operator >(const Sample& rhs){value_ > rhs.value_;}
		Sample():value_(0),age_(0){}
		Sample(const DataType& value, const size_t age):value_(value),age_(age){}
	};
	Sample samples_[Size];
	size_t ageCounter_;
};




#endif
