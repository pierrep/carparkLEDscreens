#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::exit()
{
    saveXmlSettings();
}

//--------------------------------------------------------------
void ofApp::setup(){
   // ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);

    bUseXbee = false;
    bUseSerial = false;

    loadSerialSettings();
    int baud = 115200;

    if(bUseSerial) {
        bUseSerial = serial[0].setup(serialName[0], baud);
        bUseSerial = serial[1].setup(serialName[1], baud);
        bUseSerial = serial[2].setup(serialName[2], baud);
							
        unsigned char buf[10];
        serial[0].writeBytes(buf,10);
        serial[1].writeBytes(buf,10);
        serial[2].writeBytes(buf,10);
    }

    setupCalendar();
}

//--------------------------------------------------------------
void ofApp::setupCalendar()
{
    // currently basic.ics is being downloaded from here
    // "https://www.google.com/calendar/ical/christopherbaker.net_91ul9n5dq2b6pkmin511q3bq14%40group.calendar.google.com/public/basic.ics";

    // update it every minute
    calendar = ICalendar::makeShared("https://calendar.google.com/calendar/ical/d8fpibcns53u907i6eedo65nu0%40group.calendar.google.com/private-d699b602410a90afdcbe206673832aba/basic.ics", 6000);

    calendar->reload();
    calendar->startThread();

    watcher = ICalendarWatcher::makeShared(calendar);

    watcher->registerAllEvents(this); // register this class for all events

    // set up the watcher
    ofRectangle window;
    window.setFromCenter(ofGetWidth() / 2, ofGetHeight() / 2, 480, ofGetHeight()-80);
    calendarWidget = CalendarWidget::makeShared(calendar, window);

    bInited = false;
}

//--------------------------------------------------------------
void ofApp::update(){

    if((!bInited) && (ofGetFrameNum() > 300)) {
        bInited = true;
        loadXmlSettings();
    }
	

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackgroundGradient(ofColor::white, ofColor::black);

    if (calendarWidget)
    {
        calendarWidget->draw();
    }

    int y = 50;

    std::deque<std::string>::const_iterator iter = messages.begin();

    ofDrawBitmapStringHighlight("Events:", 20, y);

    y += 10;

    while (iter != messages.end())
    {
        y += 14;

        ofDrawBitmapString(*iter, 20, y);

        if (y > ofGetHeight()) break;

        ++iter;
    }

    ofSetColor(255,0,0);
   // ofDrawBitmapString("click to test serial.",20,20);

    for(int i = 0 ;i < NUM_SCREENS;i++) {
		
        ofDrawBitmapString(content[i],300 * i+20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::writeWord(int index)
{
    ofLogNotice() << "screen" << index << ": " << content[index];

    if(bUseSerial)
    {
        unsigned char buf[255];
        for(unsigned int i = 0; i < content[index].length();i++)
        {
            buf[i] = content[index].c_str()[i];
        }
        if(index == 0)
            serial[0].writeBytes(buf,content[index].length());
        else if(index == 1)
            serial[1].writeBytes(buf,content[index].length());
        else if(index == 2)
            serial[2].writeBytes(buf,content[index].length());
    }
}

//--------------------------------------------------------------
void ofApp::writeWord(int index, string newstring)
{    
    content[index] = newstring;
			
    writeWord(index);
}

//--------------------------------------------------------------
void ofApp::loadSerialSettings()
{
    if(xml.load("settings.xml") ){
        ofLogNotice() << "settings.xml loaded!";
	} else {
        ofLogWarning() << "unable to load settings.xml";
	}
    
    if(xml.exists("//SERIAL0")) {
        serialName[0] = xml.getValue<string>("//SERIAL0");
    } else {
        serialName[0]  = "/dev/ttyACM0";
    }
       
    if(xml.exists("//SERIAL1")) {
        serialName[1] 	= xml.getValue<string>("//SERIAL1");
    } else {
        serialName[1]  = "/dev/ttyACM1";
    }
    
    if(xml.exists("//SERIAL2")) {
        serialName[2] 	= xml.getValue<string>("//SERIAL2");
    } else {
        serialName[2]  = "/dev/ttyACM2";
    }		  
        
    for(int i = 0; i < NUM_SCREENS; i++) {
		ofLogNotice("loadSettings") << "Port " << i << ": " << serialName[i] ;
	}

}

//--------------------------------------------------------------
void ofApp::loadXmlSettings()
{
    if(xml.exists("//SCREEN1")) {
        content[0] 	= xml.getValue<string>("//SCREEN1");
    }
    if(xml.exists("//SCREEN2")) {
        content[1] 	= xml.getValue<string>("//SCREEN2");
    }
    if(xml.exists("//SCREEN3")) {
        content[2] 	= xml.getValue<string>("//SCREEN3");
    }
    for(int i = 0; i < NUM_SCREENS; i++) {
        writeWord(i);
    }
}

//--------------------------------------------------------------
void ofApp::saveXmlSettings()
{
    xml.setTo("CONTENT");

    xml.setValue("SCREEN1", content[0]);
    xml.setValue("SCREEN2", content[1]);
    xml.setValue("SCREEN3", content[2]);

    xml.save("settings.xml");

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
    if(key == '1') writeWord(0,"testing screen 1");
    if(key == '2') writeWord(1,"testing screen 2");
    if(key == '3') writeWord(2,"testing screen 3");

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
    messages.push_front(msg.message);

    while (messages.size() > MAX_MESSAGES)
    {
        messages.pop_back(); // throw away the oldest.
    }
}

//--------------------------------------------------------------
void ofApp::processInstance(const ICalendarEventInstance& instance)
{
    if (instance.isValidEventInstance())
    {
        std::string description = instance.getEvent().getDescription();

        std::vector<std::string> lines = ofSplitString(description, "\n", true, true);

        std::vector<std::string>::iterator iter = lines.begin();

        while (iter != lines.end())
        {
            std::vector<std::string> tokens = ofSplitString(*iter, "=", true, true);

            if (tokens.size() == 2)
            {
                if ((tokens[0] == "screen1") || (tokens[0] == "SCREEN1"))
                {
                   writeWord(0,tokens[1]);
                }
                else if ((tokens[0] == "screen2") || (tokens[0] == "SCREEN2"))
                {
                   writeWord(1,tokens[1]);
                }
                else if ((tokens[0] == "screen3") || (tokens[0] == "SCREEN3"))
                {
                   writeWord(2,tokens[1]);
                }
                else
                {
                    ofLogError("ofApp::processInstance") << "Unknown key.";
                }
            }

            ++iter;
        }
        saveXmlSettings();
    }
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherEventAdded(const ICalendarEventInstance& instance)
{
    ofSendMessage("ADDED: " + instance.getEvent().getSummary() );

    processInstance(instance);
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherEventRemoved(const ICalendarEventInstance& instance)
{
    // note, there is a good chance this event will be invalid
    ofSendMessage("REMOVED: " + instance.getEvent().getUID());
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherEventModified(const ICalendarEventInstance& instance)
{
    ofSendMessage("MODIFIED: " + instance.getEvent().getSummary() );

    processInstance(instance);
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherEventStarted(const ICalendarEventInstance& instance)
{
    ofSendMessage("STARTED: " + instance.getEvent().getSummary() );

    processInstance(instance);
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherEventEnded(const ICalendarEventInstance& instance)
{
    ofSendMessage("ENDED: " + instance.getEvent().getSummary() );
}

//--------------------------------------------------------------
void ofApp::onCalendarWatcherError(const Poco::Exception& exception)
{
    ofSendMessage("ERROR: " + exception.displayText());
}

