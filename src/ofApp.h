#pragma once

#include "ofMain.h"
#include "WordPoetry.h"

#define NUM_WORDS 5

class ofApp : public ofBaseApp{
	
	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void loadSettings();
        void writeWord(int index);
		

        WordPoetry  poetry;

        ofSerial	serial[NUM_WORDS];
        ofSerial	xbee;
        string		serialName[NUM_WORDS];
        bool		bSendSerialMessage;
        bool        bUseSerial;
        bool        bUseXbee;
        
        
		ofXml 		xml;
        string w[NUM_WORDS];

};

