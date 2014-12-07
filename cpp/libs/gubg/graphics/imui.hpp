#ifndef HEADER_gubg_graphics_imui_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graphics_imui_hpp_ALREADY_INCLUDED

#include "gubg/point/point.hpp"
#include "gubg/graphics/imui_key.hpp"
#include "gubg/Location.hpp"
#include <deque>
#include <map>
#include <memory>

//Provides support for immediate mode user interfaces: instead of linking events to callbacks etc., a single instance of (a derived class from) IMUI
//keeps track of user input, while widgets are drawn and handled immediately, each time we cycle through the main loop.
//Make sure you sleep somewhere, otherwise you will consume all CPU

//Typical application would be:
// * Create some derived IMUI, e.g., SDLCanvas, which returns a IMUI via getIMUI()
// * Create a single Widgets instance that will manage all widgets (you can create several ones if you like, e.g., for optimization per logical page or so)
// * If some widget needs to be drawn and handled, create a WidgetProxy via widgets.get(...), call its process() method and set an instance of the desired
//   widget into this proxy when process() returns WidgetState::Empty.
// * Handle other, widget-dependent states accordingly, e.g., WidgetState::Activated is the one return by process() when a Button is pressed

namespace gubg
{
    enum class WidgetState
    {
        Empty,        //No widget is present yet in the WidgetProxy, you should set() one
        Emerging,     //The widget is being born
        Idle,         //
        Highlighted,  //Mouse is currently hoovering over it
        Selected,     //
        Activating,   //Button: Left mouse button down
        Activated,    //Button: Left mouse button up while still hoovering over the same widget where left mouse went down
        Moving,       //Scrollbar
        ScrollDown,   //Scrollbar
        ScrollUp      //Scrollbar
    };
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
        WidgetProxy &get(const Location &, unsigned int extra = 0);

        private:
            unsigned int createId_(void *location, unsigned int extra);

            std::map<unsigned int, WidgetProxy> widgetPerId_;
    };

    //Immediate-mode user interface base class that keeps track of relevant user input
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
