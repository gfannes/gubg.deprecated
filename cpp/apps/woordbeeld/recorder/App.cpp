#include "recorder/App.hpp"

#define GUBG_MODULE_ "App"
#include "gubg/log/begin.hpp"
namespace recorder { 

    App::App():
        imui::App(640, 480, "Woordbeeld recorder"),
        sm_(*this)
    {
        sm_.process(DoInit{});
    }

    void App::render(const imui::Region &region)
    {
        for (auto evt: region.keys())
            sm_.process(evt);
        for (auto evt: region.texts())
            sm_.process(Character{(int)evt.text.unicode});
        sm_.process(Render{region});
    }

    void App::sm_enter(SM::State &s)
    {
        S();L("Entering state " << (int)s());
        switch (s())
        {
            case State::Init:
                {
                    S();
                    if (!font_.loadFromFile("anonymous_pro.ttf"))
                        return error_("Font not found");
                    L("Font was found");
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
                word_sw_.reset();
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
    void App::sm_event(SM::State &s, DoInit)
    {
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
            case State::Idle:
                quitASAP();
                break;
            case State::ReadName:
                {
                    const int backspace_char = 8;
                    const int space_char = 32;
                    const int first_valid_char = 32;
                    const int last_valid_char = 126;
                    if (false) {}
                    else if (first_valid_char <= ch.ch and ch.ch <= last_valid_char)
                    {
                        const bool leading_space = (ch.ch == space_char and word_str_.empty());
                        if (!leading_space)
                            word_str_.push_back(ch.ch);
                    }
                    else if (ch.ch == backspace_char)
                    {
                        if (!word_str_.empty())
                            word_str_.pop_back();
                    }
                }
                break;
        }
    }
    void App::sm_event(SM::State &s, Render r)
    {
        const int char_size = 30;
        const int nr_lines = 4;
        const int dy = font_.getLineSpacing(char_size);
        const int x = 0.5*dy;
        std::array<sf::Text, nr_lines> lines;
        for (auto it = lines.begin(); it != lines.end(); ++it)
        {
            it->setFont(font_);
            it->setPosition(x, (it-lines.begin())*dy);
        }
        switch (s())
        {
            case State::Idle:
                lines[0].setString("<space>: start recording");
                lines[1].setString("<q>:     quit application");
                break;
             case State::Recording:
                lines[0].setString("Recording in progress ...");
                word_sw_.mark();
                lines[2].setString(word_sw_.total_elapse_hr());
                break;
             case State::ReadName:
                {
                    lines[0].setString("Type the word you just recorded");
                    lines[1].setString(" <enter>:  accept word");
                    lines[2].setString(" <escape>: drop word");
                    lines[3].setString(word_str_);
                }
                break;
        }
        for (const auto &line: lines)
            r.region.draw(line);
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
