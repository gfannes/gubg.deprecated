#include "recorder/App.hpp"

#define GUBG_MODULE_ "App"
#include "gubg/log/begin.hpp"
namespace recorder { 

    App::App():
        imui::App(640, 480, "Woordbeeld recorder"),
        sm_(*this)
    {
    }

    void App::render(const imui::Region &region)
    {
        sm_.process(Tick{});
        for (auto evt: region.keys())
            sm_.process(evt);
        for (auto evt: region.texts())
            sm_.process(Character{(int)evt.text.unicode});
    }

    void App::sm_enter(SM::State &s)
    {
        S();L("Entering state " << (int)s());
        switch (s())
        {
            case State::CheckHW:
                {
                    S();
                    if (!sf::SoundBufferRecorder::isAvailable())
                        return error_("No sound recorder found");
                    L("Recorder was found");
                    s.changeTo(State::Idle);
                }
                break;
            case State::Idle:
                break;
            case State::Recording:
                recorder_.start();
                break;
            case State::ReadName:
                word_str_.clear();
                break;
        }
    }
    void App::sm_exit(State s)
    {
        S();L("Leaving state " << (int)s);
        switch (s)
        {
            case State::Recording:
                recorder_.stop();
                word_sound_ = recorder_.getBuffer();
                sound_.setBuffer(word_sound_);
                sound_.play();
                break;
        }
    }
    void App::sm_event(SM::State &s, Tick)
    {
        switch (s())
        {
        }
    }
    void App::sm_event(SM::State &s, const imui::Event &evt)
    {
        typedef sf::Keyboard::Key Key;
        switch (s())
        {
            case State::Idle:
                if (evt.key.code != Key::Space)
                    return;
                if (evt.type == sf::Event::KeyPressed)
                    s.changeTo(State::Recording);
                break;
            case State::Recording:
                if (evt.key.code != Key::Space)
                    return;
                if (evt.type == sf::Event::KeyReleased)
                    s.changeTo(State::ReadName);
                break;
            case State::ReadName:
                if (evt.type == sf::Event::KeyPressed)
                {
                    const auto code = evt.key.code;
                    if (false) {}
                    else if (code == Key::Return)
                    {
                        word_sound_.saveToFile(word_str_+".ogg");
                        s.changeTo(State::Idle);
                    }
                    else if (code == Key::Escape)
                    {
                        s.changeTo(State::Idle);
                    }
                }
                break;
        }
    }
    void App::sm_event(SM::State &s, const Character &ch)
    {
        switch (s())
        {
            case State::ReadName:
                if (false) {}
                else if (32 <= ch.ch and ch.ch <= 127)
                {
                    word_str_.push_back(ch.ch);
                }
                else if (ch.ch == 8)
                {
                    if (!word_str_.empty())
                        word_str_.pop_back();
                }
                break;
        }
    }
    void App::sm_event(SM::State &s, const Error &err)
    {
        S();L("Error: " << err.msg);
        error_msg_ = err.msg;
        s.changeTo(State::Error);
    }
    void App::error_(const std::string &msg)
    {
        sm_.process(Error{msg});
    }

} 
#include "gubg/log/end.hpp"
