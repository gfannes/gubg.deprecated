module vix.Model;

public import vix.model.Tab;

class Model
{
    this(string homeFolder)
    {
        tabs_ ~= new Tab(homeFolder);
    }
    Tab[] tabs_;

    void setCommandGetter(string delegate() getter){getCommandDg_ = getter;}
    string getCommand() const
    {
        if (getCommandDg_ is null)
            return "Command getter is not set!";
        return getCommandDg_();
    }

    private:
    string delegate() getCommandDg_;
}
