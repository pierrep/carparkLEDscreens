#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
   // ofSetLogLevel(OF_LOG_VERBOSE);
	bSendSerialMessage = false;
	
	loadSettings();
	serial[0].listDevices();
	//vector <ofSerialDeviceInfo> deviceList = serial[0].getDeviceList();
    int baud = 115200;

    bUseXbee = false;
    bUseSerial = false;

    if(bUseSerial) {
        bUseSerial = serial[0].setup(serialName[0], baud); //linux example
		bUseSerial = serial[1].setup(serialName[1], baud); //linux example	
		bUseSerial = serial[2].setup(serialName[2], baud); //linux example			
		bUseSerial = serial[3].setup(serialName[3], baud); //linux example	
		bUseSerial = serial[4].setup(serialName[4], baud); //linux example	
							
        unsigned char buf[10];
        serial[0].writeBytes(buf,10);
        serial[1].writeBytes(buf,10);
        serial[2].writeBytes(buf,10);
        serial[3].writeBytes(buf,10);
        serial[4].writeBytes(buf,10);              
    }

    if(bUseXbee) {
        xbee.setup("/dev/ttyUSB0",9600);
    }

    poetry.setup();

}

//--------------------------------------------------------------
void ofApp::update(){
	
	if (bSendSerialMessage){
		
        writeWord(0);
        writeWord(1);
        writeWord(2);
        writeWord(3);
        writeWord(4);
		bSendSerialMessage = false;

	}
	

    if(bUseXbee) {
        int myByte = 0;
        myByte = xbee.readByte();
        if ( myByte == OF_SERIAL_NO_DATA )
          //ofLogNotice() << "no data was read";
          ;
        else if ( myByte == OF_SERIAL_ERROR )
          ofLogNotice() << "an Xbee serial error occurred";
        else {
          //ofLogNotice() << "byte received is: " << myByte;
          if(myByte == 3) {
            writeWord(0);
          } else if (myByte == 2) {
            writeWord(1);
          } else if (myByte == 1) {
            writeWord(2);
          } else if (myByte == 4) {
            writeWord(3);
          }	else if (myByte == 5) {
            writeWord(4);
          }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(0);
    ofDrawBitmapString("click to test serial.",20,20);
    for(int i = 0 ;i < 5;i++) {
		
		ofDrawBitmapString(w[i],100 * i+20, 100);
	}
}

//--------------------------------------------------------------
void ofApp::writeWord(int index)
{    
	w[index] = poetry.getWord(index);
	while(w[index].length() > 10) 
	{
		w[index] = poetry.getWord(index);
	}
			
    ofLogNotice() << "Word: " << w[index];
    if(bUseSerial) {
        unsigned char buf[10];
        for(unsigned int i = 0; i < w[index].length();i++)
        {
            buf[i] = w[index].c_str()[i];
        }
        if(index == 0)
			serial[0].writeBytes(buf,w[index].length());
		else if(index == 1)
			serial[1].writeBytes(buf,w[index].length());
		else if(index == 2)
			serial[2].writeBytes(buf,w[index].length());
		else if(index == 3)
			serial[3].writeBytes(buf,w[index].length());
		else if(index == 4)
			serial[4].writeBytes(buf,w[index].length());									
    }
}


//--------------------------------------------------------------
void ofApp::loadSettings()
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
    
    if(xml.exists("//SERIAL3")) {
        serialName[3] 	= xml.getValue<string>("//SERIAL3");
    } else {
        serialName[3]  = "/dev/ttyACM3";
    }	    

    if(xml.exists("//SERIAL4")) {
        serialName[4] 	= xml.getValue<string>("//SERIAL4");
    } else {
        serialName[4]  = "/dev/ttyACM4";
    }	  
        
    for(int i = 0; i < NUM_WORDS; i++) {
		ofLogNotice("loadSettings") << "Port " << i << ": " << serialName[i] ;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
	if(key == '1') writeWord(0);
	if(key == '2') writeWord(1);
	if(key == '3') writeWord(2);
	if(key == '4') writeWord(3);
	if(key == '5') writeWord(4);
	//bSendSerialMessage = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	bSendSerialMessage = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	
}

