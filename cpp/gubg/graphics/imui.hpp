#ifndef gubg_graphics_imui_hpp
#define gubg_graphics_imui_hpp

#include "point.hpp"
#include "graphics/imui_key.hpp"
#include <deque>
#include <map>
#include <memory>

namespace gubg
{
    enum class MouseButton
    {
        Left,
        Middle,
        Right,
        WheelUp,
        WheelDown,
    };
    enum class ButtonState
    {
        IsUp,
        IsDown,
        IsOrWentUp,
        IsOrWentDown,
    };

    enum class WidgetState {Empty, Emerging, Idle, Highlighted, Selected, Activating, Activated, Moving, ScrollDown, ScrollUp};
    class IWidget
    {
        public:
            virtual ~IWidget(){}
            //Processes the widget (draw etc. if any is present)
            //and returns its current state
            virtual WidgetState process() = 0;
    };

    struct WidgetProxy: IWidget
    {
        //The caller _gives_ the widget object to us to be managed
        //If a new one is set, the old one will be deleted
        void set(std::auto_ptr<IWidget> widget);

        //Returns the object, if any is present, but you are not expected to delete it
        template <typename T>
            T *get()
            {
                return dynamic_cast<T*>(widget_.get());
            }

        //IWidget interface
        virtual WidgetState process();

        friend struct std::map<unsigned int, WidgetProxy>;
        WidgetProxy();
        WidgetProxy(const WidgetProxy &);
        virtual ~WidgetProxy();
        WidgetProxy &operator=(const WidgetProxy &);

        private:
        std::shared_ptr<IWidget> widget_;
    };
    struct Widgets
    {
        WidgetProxy &get(unsigned int extra = 0);

        private:
            unsigned int createId_(void *location, unsigned int extra);

            std::map<unsigned int, WidgetProxy> widgetPerId_;
    };

    //Immediate-mode user interface
    class IMUI
    {
        public:
            IMUI():
                mousePosition_(0.0, 0.0),
                somethingChanged_(false),
                lastKey_(Key::Nil){}

            //Call this fast enough to get a reasonable response time
            //Returns true if some event is waiting (not sure if this maps OK for all input devices)
            //Calling this twice without getting any event should return the same
            bool processInput();

            //Resets the changed status, which should be done after one iteration through the main loop
            void reset();

            //Returns true as soon as input is ready to be processed
            //false if it timed out
            bool waitForInput(double timeout);

            //This is a non-const method, it will reset the interal lastKey_
            Key getLastKey();

            //This is a non-const method, it will reset the internal lastKey_ if a digit was found
            bool getDigit(unsigned char &digit);

            //This is a non-const method, it will reset the internal lastKey_ if ESC was found
            bool escapeIsPressed();

            bool isMouseInside(const TwoPoint<> &region) const;

            bool checkMouseButton(MouseButton button, ButtonState cmpState);

        protected:
            bool somethingChanged_;
            //This should be provided by any derived class, implementing basic input processing
            //Keeping track of input changes is done by processInput()
            virtual bool processInput_() = 0;

            Key lastKey_;
            //When processInput() polls for key presses, it can store excess Keys in here and move them
            //later to lastKey_ one by one
            std::deque<Key> cachedKeys_;
            Point<> mousePosition_;

            struct ButtonHistory
            {
                ButtonHistory():
                    isDown_(false), changed_(false){}

                void setUp(bool b)
                {
                    if (isDown_ == b)
                        changed_ = true;
                    isDown_ = !b;
                }
                void get(bool &cur, bool &ch)
                {
                    cur = !isDown_;
                    ch = changed_;
                    changed_ = false;
                }

                private:
                bool isDown_;
                bool changed_;
            };
            ButtonHistory leftMouse_;
            ButtonHistory middleMouse_;
            ButtonHistory rightMouse_;
            ButtonHistory wheelUp_;
            ButtonHistory wheelDown_;
    };
}

#endif
