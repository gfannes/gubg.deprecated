#include "gubg/StateMachine.hpp"
#include "imui/sfml.hpp"
#include "imui/Region.hpp"
#include "SFML/Audio/SoundBufferRecorder.hpp"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Graphics/Font.hpp"
#include <string>

namespace recorder { 
    class App: public imui::App
    {
        public:
            App();
            void render(const imui::Region &) override;
        private:
            enum class State {Init, Idle, Recording, ReadName, Error};
            typedef gubg::StateMachine_ftop<App, State, State::Init> SM;
            friend SM;
            SM sm_;
            void sm_enter(SM::State &);
            void sm_exit(State);
            DATA_EVENT(DoInit);
            void sm_event(SM::State &, DoInit);
            void sm_event(SM::State &, const imui::Event &);
            DATA_EVENT(Character, int, ch);
            void sm_event(SM::State &, const Character &);
            DATA_EVENT(Render, const imui::Region &, region);
            void sm_event(SM::State &, Render);
            DATA_EVENT(Error, std::string, msg);
            void sm_event(SM::State &, const Error &);
            void error_(const std::string &);

            sf::SoundBufferRecorder recorder_;
            sf::SoundBuffer word_sound_;
            std::string word_str_;
            sf::Sound sound_;
            std::string error_msg_;

            sf::Font font_;
    };
} 
