#ifndef HEADER_woordbeeld_player_App_hpp_ALREADY_INCLUDED
#define HEADER_woordbeeld_player_App_hpp_ALREADY_INCLUDED

#include "player/Codes.hpp"
#include "gubg/StateMachine.hpp"
#include "gubg/chrono/Stopwatch.hpp"
#include "gubg/file/File.hpp"
#include "imui/App.hpp"
#include "imui/Region.hpp"
#include "SFML/Audio/SoundBufferRecorder.hpp"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Graphics/Font.hpp"
#include <string>
#include <vector>

namespace player { 
    class App: public imui::App
    {
        public:
            App();
            void render(const imui::Region &) override;
        private:
            enum class State {Init, Idle, PlayingWord, CheckingLettersKnown, ReadLetters, SpellLetters, Wrong, Error};
            typedef gubg::StateMachine_ftop<App, State, State::Init> SM;
            friend SM;
            SM sm_;
            void sm_enter(SM::State &);
            void sm_exit(State);
            DATA_EVENT(DoInit);
            void sm_event(SM::State &, DoInit);
            void sm_event(SM::State &, const imui::backend::Event &);
            DATA_EVENT(Character, int, ch);
            void sm_event(SM::State &, const Character &);
            DATA_EVENT(Render, const imui::Region &, region);
            void sm_event(SM::State &, Render);
            DATA_EVENT(Error, std::string, msg);
            void sm_event(SM::State &, const Error &);
            void error_(const std::string &);

            ReturnCode loadWordFns_();
            typedef std::vector<gubg::file::File> Files;
            Files word_fns_;

            sf::SoundBuffer word_sound_;
            std::string wanted_word_str_;
            std::string word_str_;
            sf::Sound sound_;
            gubg::chrono::Stopwatch<> idle_sw_;
            gubg::chrono::Stopwatch<> wrong_sw_;

            struct Stats
            {
                int ok = 0;
                int ko = 0;
                int refused = 0;
            };
            Stats stats_;

            std::string error_msg_;

            sf::Font font_;
    };
} 

#endif
