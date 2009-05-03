#ifndef gubg_log_hpp
#define gubg_log_hpp

#include <string>
#include <vector>

namespace gubg
{
    class Log
    {
    public:
	class Output
	{
	public:
	    class Primitive
	    {
	    public:
		virtual void write(const std::string &str) = 0;
	    };

	    Output();
	    Output(Primitive *primitive);
	    virtual ~Output();

	    virtual void newLevel(std::string &tag) = 0;
	    virtual void closeLevel() = 0;
	    virtual void newLine() = 0;
	    virtual void closeLine() = 0;
	    virtual Output &operator<<(const std::string &str) = 0;

	    Primitive *swapPrimitive(Primitive *primitive);

	private:
	    Primitive *_primitive;
	};

	virtual ~Log();

	static Log &instance();

	void add(Output *output);

    private:
	// Singleton: disallow constructor
	Log();
	// Disallow copying
	Log(const Log& log);
	Log &operator=(const Log& log);

	std::vector<Output*> _outputs;
    };
};

#endif
