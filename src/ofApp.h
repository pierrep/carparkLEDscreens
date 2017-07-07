#pragma once

#include "ofMain.h"
#include "ofxICalendar.h"
#include "CalendarWidget.h"

using namespace ofx::Time;

#define NUM_SCREENS 3

class ofApp : public ofBaseApp{
	
	public:
        void exit();
		void setup();
		void update();
		void draw();
		void keyPressed  (int key);
		void mousePressed(int x, int y, int button);
		void gotMessage(ofMessage msg);
        void onCalendarWatcherEventAdded(const ICalendarEventInstance& instance);
        void onCalendarWatcherEventRemoved(const ICalendarEventInstance& instance);
        void onCalendarWatcherEventModified(const ICalendarEventInstance& instance);
        void onCalendarWatcherEventStarted(const ICalendarEventInstance& instance);
        void onCalendarWatcherEventEnded(const ICalendarEventInstance& instance);
        void onCalendarWatcherError(const Poco::Exception& exception);
        void processInstance(const ICalendarEventInstance& instance);
        void loadXmlSettings();
        void saveXmlSettings();
        void loadSerialSettings();
        void writeWord(int index);
        void writeWord(int index, string content);
        void setupCalendar();
        string pad_right(string const& str, size_t s);

        /* calendar */
        ICalendar::SharedPtr calendar;
        ICalendarWatcher::SharedPtr watcher;

        CalendarWidget::SharedPtr calendarWidget;

        enum
        {
            MAX_MESSAGES = 500
        };

        std::deque<std::string> messages;

        ofSerial	serial[NUM_SCREENS];
        string		serialName[NUM_SCREENS];
        string      content[NUM_SCREENS];

        bool        bUseSerial;
        bool        bUseXbee;
        bool        bInited;
        bool        bReload;
        bool        bDoScroll;
        
		ofXml 		xml;

};

