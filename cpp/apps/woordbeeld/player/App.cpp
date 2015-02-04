#include "player/App.hpp"
#include "gubg/file/Filesystem.hpp"
#include <array>
#include <algorithm>
#include <random>
#include <thread>

namespace  { 
    std::random_device rd;
    std::mt19937 rng(rd());
} 

#define GUBG_MODULE_ "App"
#include "gubg/log/begin.hpp"
namespace player { 

    App::App():
        imui::App(640, 480, "Woordbeeld player"),
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
                    if (!MSS_IS_OK(loadWordFns_()))
                        return error_("Could not load word files");
                    if (word_fns_.empty())
                        return error_("No word files found");
                    L("Word files loaded: " << word_fns_.size());
                    s.changeTo(State::Idle);
                }
                break;
            case State::Idle:
                idle_sw_.reset();
                break;
            case State::PlayingWord:
                {
                    //Load a random word
                    const auto s = word_fns_.size();
                    if (s == 0)
                        return error_("No word files found");
                    std::uniform_int_distribution<> ix(0, s-1);
                    const auto word_fn = word_fns_[ix(rng)];
                    wanted_word_str_ = word_fn.basename(word_fn.extension());
                    word_sound_.loadFromFile(word_fn.name());

                    //Playback of selected word
                    sound_.setBuffer(word_sound_);
                    sound_.play();
                }
                break;
            case State::ReadLetters:
            case State::SpellLetters:
                word_str_.clear();
                break;
            case State::Wrong:
                wrong_sw_.reset();
                break;
        }
    }
    void App::sm_exit(State s)
    {
        S();L("Leaving state " << (int)s);
        switch (s)
        {
        }
    }
    void App::sm_event(SM::State &s, DoInit)
    {
    }
    void App::sm_event(SM::State &s, const imui::backend::Event &evt)
    {
        typedef sf::Keyboard::Key Key;
        typedef imui::backend::Event Event;
        switch (s())
        {
            case State::Idle:
                break;
            case State::CheckingLettersKnown:
                if (evt.type == Event::KeyPressed)
                {
                    const auto code = evt.key.code;
                    if (false) {}
                    else if (code == Key::Escape)
                    {
                        ++stats_.refused;
                        s.changeTo(State::Idle);
                    }
                }
                break;
            case State::ReadLetters:
            case State::SpellLetters:
                if (evt.type == Event::KeyPressed)
                {
                    const auto code = evt.key.code;
                    if (false) {}
                    else if (code == Key::Return)
                    {
                        if (wanted_word_str_ != word_str_)
                        {
                            ++stats_.ko;
                            s.changeTo(State::Wrong);
                        }
                        else
                        {
                            ++stats_.ok;
                            s.changeTo(State::Idle);
                        }
                    }
                    else if (code == Key::Escape)
                    {
                        if (word_str_.empty())
                        {
                            ++stats_.refused;
                            s.changeTo(State::Idle);
                        }
                        else
                            word_str_.clear();
                    }
                }
                break;
        }
    }
    void App::sm_event(SM::State &s, const Character &ch)
    {
        const int backspace_char = 8;
        const int space_char = 32;
        const int first_valid_char = 32;
        const int last_valid_char = 126;

        switch (s())
        {
            case State::Idle:
            case State::Error:
                if (ch.ch == 'q')
                    quitASAP();
                break;
            case State::CheckingLettersKnown:
                switch (ch.ch)
                {
                    case 'y':
                    case 'Y':
                        s.changeTo(State::ReadLetters);
                        break;
                    case 'n':
                    case 'N':
                        s.changeTo(State::SpellLetters);
                        break;
                    case 'q':
                        quitASAP();
                        break;
                }
                break;
            case State::ReadLetters:
                {
                    if (false) {}
                    else if (first_valid_char <= ch.ch && ch.ch <= last_valid_char)
                    {
                        const bool leading_space = (ch.ch == space_char && word_str_.empty());
                        if (!leading_space)
                            word_str_.insert(0, 1, ch.ch);
                    }
                    else if (ch.ch == backspace_char)
                    {
                        if (!word_str_.empty())
                            word_str_.erase(0, 1);
                    }
                }
                break;
             case State::SpellLetters:
                {
                    if (false) {}
                    else if (first_valid_char <= ch.ch && ch.ch <= last_valid_char)
                    {
                        const bool leading_space = (ch.ch == space_char && word_str_.empty());
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
        const int nr_lines = 6;
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
                {
                    idle_sw_.mark();
                    {
                        std::ostringstream oss;
                        oss << "ok: " << stats_.ok << ", wrong: " << stats_.ko << ", refused: " << stats_.refused;
                        lines[0].setString(oss.str());
                    }
                    lines[1].setString(idle_sw_.total_elapse_hr());
                    lines[3].setString("<q>:     quit application");

                    if (idle_sw_.total_elapse() >= std::chrono::seconds(1))
                        s.changeTo(State::PlayingWord);
                }
                break;
            case State::PlayingWord:
                {
                    lines[0].setString("Playing word");
                    if (sound_.getStatus() == sf::Sound::Stopped)
                        s.changeTo(State::CheckingLettersKnown);
                }
                break;
            case State::CheckingLettersKnown:
                {
                    lines[0].setString("Do you know the letters?");
                    lines[2].setString("<y>:       yes");
                    lines[3].setString("<n>:       no");
                    lines[4].setString("<escape>:  skip");
                    lines[5].setString("<q>:     quit application");
                }
                break;
             case State::ReadLetters:
                {
                    lines[0].setString("Type the word backwards");
                    {
                        std::string str(word_str_.size(), '*');
                        lines[2].setString(str);
                    }
                    lines[4].setString("<return>:  submit");
                    lines[5].setString("<escape>:  clear/skip");
                }
                break;
             case State::SpellLetters:
                {
                    lines[0].setString("Please type: " + wanted_word_str_);
                    lines[2].setString(word_str_);
                    lines[4].setString("<return>:  submit");
                    lines[5].setString("<escape>:  clear/skip");
                }
                break;
            case State::Wrong:
                {
                    lines[0].setString("WRONG");
                    lines[2].setString("wanted: " + wanted_word_str_);
                    lines[3].setString("actual: " + word_str_);

                    wrong_sw_.mark();
                    if (wrong_sw_.total_elapse() >= std::chrono::seconds(2))
                        s.changeTo(State::Idle);
                }
                break;
            case State::Error:
                lines[0].setString(std::string("ERROR: ")+error_msg_);
                lines[2].setString("<q>:     quit application");
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

    ReturnCode App::loadWordFns_()
    {
        MSS_BEGIN(ReturnCode);
        using namespace gubg::file;
        auto cwd = getcwd();
        L(STREAM(cwd.name()));
        MSS(read(word_fns_, getcwd()));
        auto new_end = std::remove_if(word_fns_.begin(), word_fns_.end(), [](const File &file){ return file.extension() != "ogg"; });
        word_fns_.erase(new_end, word_fns_.end());
        MSS_END();
    }

} 
#include "gubg/log/end.hpp"
