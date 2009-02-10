module rinle.rinle;

import gubg.ui;
import gubg.ncurses;

class Rinle
{
    this()
        {
            mNCurses = new NCurses;
            mInput = mNCurses;
            mOutput = mNCurses;
            mOutput.refresh;
        }
    ~this()
        {
            delete mNCurses;
        }

private:
    NCurses mNCurses;
    Input mInput;
    Output mOutput;
}

int main()
{
    scope rinle = new Rinle;
    Thread.sleep(1);
    return 0;
}

