module sommen;

import gubg.graphics.Canvas;
import gubg.Timer;
import gubg.Format;
import core.thread;
import std.random;
import std.stdio;

version = drie;

class Question
{
    this (uint max)
    {
        max_ = max;
    }

    void reset()
    {
        //Generate a new question and keep track of it
        answer_ = uniform(2, max_+1);
        version (twee)
        {
            uint lhs = uniform(1, answer_);
            uint rhs = answer_ - lhs;
            question_ = Format.immediate("%s + %s = ?", lhs, rhs);
        }
        version (drie)
        {
            uint lhs = uniform(1, answer_);
            uint mid = uniform(0, answer - lhs);
            uint rhs = answer_ - lhs - mid;
            question_ = Format.immediate("%s + %s + %s = ?", lhs, mid, rhs);
        }
    }

    string toString(){return question_;}
    uint answer(){return answer_;}

    private:
    uint max_;
    string question_;
    uint answer_;
}

class Score
{
    this (uint total)
    {
        total_ = total;
        reset();
        auto tmp = Style().stroke(Color.white).width(3);
        styleUnknown_ = styleCorrect_ = styleIncorrect_ = tmp;
        styleUnknown_.fill(Color.gray);
        styleCorrect_.fill(Color.green);
        styleIncorrect_.fill(Color.red);
    }

    void reset()
    {
        scores_ = new Result[total_];
        currentIX_ = 0;
        foreach (ref r; scores_)
            r = Result.Unknown;
    }

    void register(bool ok)
    {
        scores_[currentIX_++] = (ok ? Result.Correct : Result.Incorrect);
    }
    bool isComplete()
    {
        return currentIX_ == scores_.length;
    }
    uint nrCorrect()
    {
        uint res = 0;
        foreach (score; scores_)
        {
            if (Result.Correct == score)
                ++res;
        }
        return res;
    }

    void draw(ICanvas canvas)
    {
        auto w = canvas.width/(total_+1);
        foreach (ix, score; scores_)
        {
            Style *style;
            switch (score)
            {
                case Result.Unknown:   style = &styleUnknown_; break;
                case Result.Correct:   style = &styleCorrect_; break;
                case Result.Incorrect: style = &styleIncorrect_; break;
            }
            canvas.drawCircle(Point((ix+1)*w, canvas.height-w), 0.3*w, *style);
        }
    }

    private:
    uint total_;
    enum Result {Unknown, Correct, Incorrect};
    Result[] scores_;
    uint currentIX_;
    Style styleUnknown_;
    Style styleCorrect_;
    Style styleIncorrect_;
}

scope class Game
{
    this (uint width, uint height)
    {
        //Initialize graphics-related things
        {
            width_ = width;
            height_ = height;
            center_ = Point(0.5*width_, 0.5*height_);
            canvas_ = new SDLCanvas(width_, height_);
            canvas_.setFont("Serif");
            textStyle_.fill(Color.purple).stroke(Color.purple).width(10);
            textStyleOK_.fill(Color.green).stroke(Color.green).width(10);
            textStyleKO_.fill(Color.red).stroke(Color.red).width(10);
            timeoutStyle_.fill(Color.purple);
        }
        state_ = State.Start;
        timer_ = Timer(ResetType.NoAuto);
        timeout_ = 10.0;
        score_ = new Score(10);
        question_ = new Question(8);
    }

    bool process()
    {
        scope ds = canvas_.new DrawScope;
        switch (state_)
        {
            case State.Start:
                changeState_(State.ShowSplashScreen);
                break;
            case State.ShowSplashScreen:
                if (timer_.difference > 5)
                {
                    changeState_(State.StartGame);
                    break;
                }
                if (canvas_.imui.escapeIsPressed())
                {
                    changeState_(State.End);
                    break;
                }
                canvas_.drawText("Welkom bij sommen.", TwoPoint.centered(center_, 0.9*width_, 0.115*height_), VAlign.Center, HAlign.Left, textStyle_);
                break;
            case State.StartGame:
                score_.reset();
                changeState_(State.AskQuestion);
                break;
            case State.AskQuestion:
                if (score_.isComplete())
                {
                    changeState_(State.ShowScore);
                    break;
                }
                auto elapse = timer_.difference;
                if (elapse > timeout_)
                    changeState_(State.AnswerTooLate);
                score_.draw(canvas_);
                canvas_.drawText(question_.toString(), TwoPoint.centered(center_, 0.5*width_, 0.1*height_), VAlign.Center, HAlign.Center, textStyle_);
                //Draw the timout bar
                {
                    real f = 0.03;
                    auto barWidth = (1.0-2*f)*canvas_.width;
                    auto barHeight = 0.02*canvas_.width;
                    auto xLeft = f*canvas_.width;
                    auto yBottom = f*canvas_.height;
                    canvas_.drawRectangle(TwoPoint(xLeft, yBottom, xLeft + ((timeout_ - elapse)/timeout_)*barWidth, yBottom + barHeight), timeoutStyle_);
                }
                if (canvas_.imui.escapeIsPressed())
                {
                    changeState_(State.End);
                    break;
                }
                //Check if a numeric key was pressed and compare it with the expected answer
                {
                    ubyte digit;
                    if (canvas_.imui.getDigit(digit))
                    {
                        bool answerIsCorrect = question_.answer() == digit;
                        score_.register(answerIsCorrect);
                        changeState_((answerIsCorrect ? State.AnswerOK : State.AnswerKO));
                    }
                }
                break;
            case State.AnswerOK:
                if (timer_.difference > 2.0)
                    changeState_(State.AskQuestion);
                canvas_.drawText("Goed zo!", TwoPoint.centered(center_, 0.5*width_, 0.1*height_), VAlign.Center, HAlign.Center, textStyleOK_);
                break;
            case State.AnswerKO:
                if (timer_.difference > 3.0)
                    changeState_(State.AskQuestion);
                canvas_.drawText(Format.immediate("Fout, het was %s", question_.answer()), TwoPoint.centered(center_, 0.5*width_, 0.1*height_), VAlign.Center, HAlign.Center, textStyleKO_);
                break;
            case State.AnswerTooLate:
                score_.register(false);
                changeState_(State.AskQuestion);
                break;
            case State.ShowScore:
                canvas_.drawText(Format.immediate("%s juist!", score_.nrCorrect), TwoPoint.centered(center_, 0.5*width_, 0.1*height_), VAlign.Center, HAlign.Center, textStyle_);
                if (timer_.difference > 5)
                    changeState_(State.StartGame);
                break;
            case State.End:
                writeln("This is the final state");
                return false;
                break;
        }
        return true;
    }

    private:
    uint width_;
    uint height_;
    Point center_;
    SDLCanvas canvas_;
    Style textStyle_;
    Style textStyleOK_;
    Style textStyleKO_;
    Style timeoutStyle_;

    //The state machine
    enum State
    {
        Start, ShowSplashScreen, StartGame, AskQuestion, AnswerOK, AnswerKO, AnswerTooLate, ShowScore, End
    }
    State state_;
    void changeState_(State newState)
    {
        switch (newState)
        {
            case State.ShowSplashScreen:
                writeln("Changing state to ShowSplashScreen");
                timer_.reset();
                break;
            case State.AskQuestion:
                timer_.reset();
                question_.reset();
                break;
            case State.AnswerOK:
                timer_.reset();
                break;
            case State.AnswerKO:
                timer_.reset();
                break;
            case State.ShowScore:
                timer_.reset();
                break;
            default:
                break;
        }
        state_ = newState;
    }

    Timer timer_;
    real timeout_;
    Score score_;
    Question question_;
}

void main()
{
    writeln("Welkom bij \"sommen\".");

    auto f = 2;
    scope game = new Game(f*640, f*480);

    while (game.process())
    {
        Thread.sleep(100000);
    }

    writeln("Game over.");
}
