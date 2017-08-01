#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::exit()
{
    saveXmlSettings();
}

string ofApp::pad_right(string const& str, size_t s)
{

    if ( str.size() < s )
        return str + std::string(s-str.size(), ' ');
    else
        return str;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);

    bUseXbee = false;
    bUseSerial = true;
    bReload = false;
    bDoScroll = false;

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
    //calendar = ICalendar::makeShared("https://calendar.google.com/calendar/ical/d8fpibcns53u907i6eedo65nu0%40group.calendar.google.com/private-d699b602410a90afdcbe206673832aba/basic.ics", 6000);
    calendar = ICalendar::makeShared("https://calendar.google.com/calendar/ical/g3onnkiiiadekb12ap491b8u4k%40group.calendar.google.com/private-c898ee38be7b00d3b41200a888b73932/basic.ics", 6000);

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
	curTime = ofGetElapsedTimeMillis();
	
    if((!bInited) && (ofGetFrameNum() > 300)) {
        bInited = true;
        loadXmlSettings();
    }
	else
    if((bReload) && (curTime - prevTime > 4000)) {
		ofLogNotice() << "Do Reload...";
        loadXmlSettings();
        bReload = false;
    }
	else
	if(bUseSerial) {
		for(int i = 0; i < 3; i++) {
			int receiveByte = 0;
			receiveByte = serial[i].readByte();
			if(receiveByte == 69) {
				ofLogNotice() << "Error...Resend text!";
				bReload = true;
				curTime = prevTime = ofGetElapsedTimeMillis();
				break;
			}
		}
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
    ofLogNotice() << "screen" << index << ": " << content[index] << " length: " << content[index].length();

    if(bUseSerial)
    {
        unsigned char buf[255];
        buf[0] = (uint8_t) 0
        ;
        unsigned int len = content[index].length();
        for(unsigned int i = 1; i < len+1;i++)
        {
            buf[i] = content[index].c_str()[i-1];
        }

        buf[0] = (uint8_t) len;
       // cout << "buf[0]" << buf[0] << endl;
        len += 1;
        if(index == 0)
            serial[0].writeBytes(buf,len);
        else if(index == 1)
            serial[1].writeBytes(buf,len);
        else if(index == 2)
            serial[2].writeBytes(buf,len);
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
	ofLogNotice() << "loading XML";
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

    if(bDoScroll)
    {
        size_t padlen = 0;
	if(content[0].size() > content[1].size())
        {
	    padlen = content[0].size();
        } else {
	    padlen = content[1].size();
        }

	if(content[2].size() > padlen)
        {
	    padlen = content[2].size();
        } else {
	    padlen = padlen;
        }
	
        content[0] = pad_right(content[0], padlen);
        content[1] = pad_right(content[1], padlen);
        content[2] = pad_right(content[2], padlen);
    }
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

        bDoScroll = false;

        while (iter != lines.end())
        {
            std::vector<std::string> tokens = ofSplitString(*iter, "=", true, true);

            if (tokens.size() == 2)
            {
                if ((tokens[0] == "screen1") || (tokens[0] == "SCREEN1"))
                {
                   //writeWord(0,tokens[1]);
		     content[0] = tokens[1];
                }
                else if ((tokens[0] == "screen2") || (tokens[0] == "SCREEN2"))
                {
                   //writeWord(1,tokens[1]);
		     content[1] = tokens[1];
                }
                else if ((tokens[0] == "screen3") || (tokens[0] == "SCREEN3"))
                {
                   //writeWord(2,tokens[1]);
		     content[2] = tokens[1];
                }
                else if ((tokens[0] == "scroll") || (tokens[0] == "SCROLL"))
                {
                     bDoScroll = true;
                }
                else
                {
                    ofLogError("ofApp::processInstance") << "Unknown key.";
                }
            }

            ++iter;
        }
        saveXmlSettings();
    curTime = prevTime = ofGetElapsedTimeMillis();
	bReload = true;
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

