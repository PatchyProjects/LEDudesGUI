/**************************************************************
***** FrameProcessor written by Christopher Hall   ************
***** hall@bu.edu                                  ************ 
***** Boston University ECE Senior Design 2012-13  ************
**************************************************************/

#pragma once
#include <cv.h>
#include "Image.h"
//#include "Rect.h"
#include <vector>
#include <iostream>
#include <math.h>
#include <string>


using namespace std;

/* Bounderies for area of LED locations */
struct LED
{
	int topX;
	int topY;
	int bottomX;
	int bottomY;
};

/*	TAG FRAME LED CONSTANTS	*/
	static Color LED1Tag = Color(255,0,0);
	static Color LED2Tag = Color(0,255,0);
	static Color LED3Tag = Color(0,0,255);
	static Color LED4Tag = Color(255,255,0);
	static Color LED5Tag = Color(255,0,255);
	static Color LED6Tag = Color(0,255,255);
	static Color LED7Tag = Color(128,0,0);
	static Color LED8Tag = Color(0,128,0);
	static Color LED9Tag = Color(0,0,128);
	static Color LED10Tag = Color(128,128,0);
	static Color LED11Tag = Color(128,0,128);
	static Color LED12Tag = Color(0,128,128);
	static Color LED13Tag = Color(255,128,0);
	static Color LED14Tag = Color(0,255,128);
	static Color LED15Tag = Color(255,0,128);
	static Color LED16Tag = Color(128,0,255);
	static Color BorderTag = Color(128,128,128);

class FrameProcessor
{
	public:
	
	/**********************************
		For processed replay **********
	**********************************/
	vector<vector<int>> processedRVals;
	vector<vector<int>> processedGVals;
	vector<vector<int>> processedBVals;

	void replayVideo(Mat& frame, vector<int> r, vector<int> g, vector<int> b)
	{
		processed(frame);
		int ledLoc = 0; //led location, max = 16;
		
		//colorCorrect();
		int HEIGHT = processed.getHeight();
		int WIDTH = processed.getWidth();
		

			for(int i = 0; i<4; i++)
			{
				for(int j = 0; j<4; j++)
				{
					int x = j*WIDTH/4;
					int y = i*HEIGHT/4;
					rectangle( frame,
								Point( x, y ),
								Point( x+WIDTH/4, y+HEIGHT/4),
								Scalar( b[ledLoc], g[ledLoc], r[ledLoc] ),		// Color in BGR format
								-1,
								8 );

					ledLoc++;
				} //j
			} //i
			
			imshow("processed", processed.getImage());		//show the processed image
			cvWaitKey(1);

	}



	void replayVideo(Mat& frame)
	{
		processed(frame);
		int ledLoc = 0; //led location, max = 16;
		
		//colorCorrect();
		int HEIGHT = processed.getHeight();
		int WIDTH = processed.getWidth();
		
		for(int frameNum = 0; frameNum < processedRVals.size(); frameNum++)
		{
			for(int i = 0; i<4; i++)
			{
				for(int j = 0; j<4; j++)
				{
					for(int x = j*WIDTH/4; x < (j+1)*WIDTH/4; x++)
					{
						for(int y = i*HEIGHT/4; y < (i+1)*HEIGHT/4; y++)
						{
							processed.set(x,y,processedRVals[frameNum][ledLoc],processedGVals[frameNum][ledLoc],processedBVals[frameNum][ledLoc]);
						}
					}
					ledLoc++;
				} //j
			} //i
			
			imshow("processed", processed.getImage());		//show the processed image
			cvWaitKey(1);
			ledLoc = 0;
		} // frame
	}

	FrameProcessor():
	  myChar(0)
	{
		//make a new window to show our processed results
		namedWindow("processed",1);
	}

	vector<LED> ledLocationVec;
	vector<int> rFrameVals;
	vector<int> bFrameVals;
	vector<int> gFrameVals;

	void printLocations()
	{
		for(int i = 0; i<ledLocationVec.size(); i++)
		{
			cout<<"LED"<<i+1<<endl;
			cout<<"Top("<<ledLocationVec[i].topX<<","<<ledLocationVec[i].topY<<")"<<endl;
			cout<<"Bottom("<<ledLocationVec[i].bottomX<<","<<ledLocationVec[i].bottomY<<")"<<endl;
		}
	}
	
	void scaleTagLocations(Mat &frame,Mat &tagFrameMat)
	{
		tagFrame(tagFrameMat);
		double tagWIDTH = tagFrame.getWidth();
		double tagHEIGHT = tagFrame.getHeight();
		processed(frame);							// load video frame to get video dimensions
		double WIDTH = processed.getWidth();
		double HEIGHT = processed.getHeight();
		
		double wRatio = WIDTH/tagWIDTH;
		double hRatio = HEIGHT/tagHEIGHT;

		for(int i = 0; i< ledLocationVec.size(); i++)
		{
			ledLocationVec[i].topX *= wRatio;
			ledLocationVec[i].bottomX *= wRatio;
			ledLocationVec[i].topY *= hRatio;
			ledLocationVec[i].bottomY *= hRatio;
		}

	}
	void locateLEDLocations(Mat &frame)
	{
		tagFrame(frame);

		vector<Color> ledTagColors;
		ledTagColors.push_back(LED1Tag);
		ledTagColors.push_back(LED2Tag);
		ledTagColors.push_back(LED3Tag);
		ledTagColors.push_back(LED4Tag);
		ledTagColors.push_back(LED5Tag);
		ledTagColors.push_back(LED6Tag);
		ledTagColors.push_back(LED7Tag);
		ledTagColors.push_back(LED8Tag);
		ledTagColors.push_back(LED9Tag);
		ledTagColors.push_back(LED10Tag);
		ledTagColors.push_back(LED11Tag);
		ledTagColors.push_back(LED12Tag);
		ledTagColors.push_back(LED13Tag);
		ledTagColors.push_back(LED14Tag);
		ledTagColors.push_back(LED15Tag);
		ledTagColors.push_back(LED16Tag);

		int HEIGHT = tagFrame.getHeight();
		int WIDTH = tagFrame.getWidth();

		for(int i = 0; i < ledTagColors.size(); i++)
		{
			bool tagFound = false;
			LED foundLED;	// I really should make LED a class instead of a struct
			for(int j = 0; j < WIDTH; j++)
			{
				for(int k = 0; k < HEIGHT; k++)
				{
					Color currentColor = tagFrame.get(j,k);
					tagFound = isColorMatch(currentColor,ledTagColors[i]);
					if(tagFound)
					{
						foundLED.topX = j;
						foundLED.topY = k;
						foundLED.bottomX = j + getLEDWidth(frame,ledTagColors[i],j+5,k+5);
						foundLED.bottomY = k + getLEDHeight(frame,ledTagColors[i],j+5,k+5);
						break;
					}
				} // k
				if(tagFound)
					break;
			} // j
			if(!tagFound)
			{
				foundLED.topX = 0;		// NEED TO MAKE SPECIAL CASE SO LED IS ALWAYS OFF IN THIS CASE
				foundLED.topY = 0;
				foundLED.bottomX = 0;
				foundLED.bottomY = 0;
			}
			ledLocationVec.push_back(foundLED);
		} // i

	}
	
	void basicLayout(Mat& frame)
	{	
		processed(frame);
		int HEIGHT = processed.getHeight();
		int WIDTH = processed.getWidth();
		cout<<"height: "<<HEIGHT<<"width: "<<WIDTH<<endl;
		for(int i = 0; i<16; i++)
		{
			int topX = WIDTH/4*(i%4);
			int topY = HEIGHT/4*((i/4)%4);
			int bottomX = topX + WIDTH/4;
			int bottomY = topY + HEIGHT/4;
			LED tempLED;
			tempLED.topX = topX;
			tempLED.topY = topY;
			tempLED.bottomX = bottomX;
			tempLED.bottomY = bottomY;
			
			ledLocationVec.push_back(tempLED);
		}
	 }
	
	bool process(Mat& frame)
	{
		vector<int> rVec;
		vector<int> gVec;
		vector<int> bVec;

		processed(frame);
		
		for(int i = 0; i<16; i++)
		{
			LED currentSection = ledLocationVec[i];
			int rAvg = 0;
			int gAvg = 0;
			int bAvg = 0;
			for(int x = currentSection.topX; x<currentSection.bottomX; x++)
			{
				for(int y = currentSection.topY; y<currentSection.bottomY; y++)
				{
					rAvg += processed.getR(x,y);
					gAvg += processed.getG(x,y);
					bAvg += processed.getB(x,y);
				} //k
			} //j
			int sectionArea = (currentSection.bottomX-currentSection.topX)*(currentSection.bottomY-currentSection.topY);
			if(sectionArea != 0) // Prevents divide by 0 in case section isn't present
			{
				rAvg = rAvg/sectionArea;
				gAvg = gAvg/sectionArea;
				bAvg = bAvg/sectionArea;
			}
			rVec.push_back(rAvg);
			gVec.push_back(gAvg);
			bVec.push_back(bAvg);
		} //i
		rFrameVals = rVec;
		gFrameVals = gVec;
		bFrameVals = bVec;
		
		colorCorrect();
		
		processedRVals.push_back(rFrameVals);
		processedGVals.push_back(gFrameVals);
		processedBVals.push_back(bFrameVals);
		
		int ledLoc = 0; //led location, max = 16;
		
		
		int HEIGHT = processed.getHeight();
		int WIDTH = processed.getWidth();

		for(int i = 0; i<4; i++)
		{
				for(int j = 0; j<4; j++)
				{
					int x = j*WIDTH/4;
					int y = i*HEIGHT/4;
					rectangle( frame,
								Point( x, y ),
								Point( x+WIDTH/4, y+HEIGHT/4),
								Scalar( bFrameVals[ledLoc], gFrameVals[ledLoc], rFrameVals[ledLoc] ),		// Color in BGR format
								-1,
								8 );

					ledLoc++;
				} //j
		} //i
		/*
		for(int i = 0; i<4; i++)
		{
			for(int j = 0; j<4; j++)
			{
				for(int x = j*WIDTH/4; x < (j+1)*WIDTH/4; x++)
				{

					for(int y = i*HEIGHT/4; y < (i+1)*HEIGHT/4; y++)
					{
						processed.set(x,y,rFrameVals[ledLoc],gFrameVals[ledLoc],bFrameVals[ledLoc]);
					}
				}
				ledLoc++;
			} //j
		} //i
		*/
		processed(frame);
		//show the processed image
		imshow("processed", processed.getImage());
		cvWaitKey(1);

		return true;
	}
	
	// Applies gamma correction on the frame
	void colorCorrect()
	{
		for(int i = 0; i < rFrameVals.size(); i++)
		{
			rFrameVals[i] = gammaCorrect(rFrameVals[i]);
			gFrameVals[i] = gammaCorrect(gFrameVals[i]);
			bFrameVals[i] = gammaCorrect(bFrameVals[i]);
		}
	}
	
	int gammaCorrect(int colorVal)
	{
		float gammaVal = 2;

		int correctedVal = 255 * pow( ((float)colorVal)/255,1/gammaVal);

		return correctedVal;
	}

	// creates string corresponding to RGB values of frame 
	// string is in Hex format: R1...R16,G1...G16,B1...B16
	String getFrameString()
	{
		String frameStr;
		for(int i = 0; i<rFrameVals.size(); i++)
		{
			frameStr += intToHex(rFrameVals[i]);
			frameStr += ",";
		}
		for(int i = 0; i<gFrameVals.size(); i++)
		{
			frameStr += intToHex(gFrameVals[i]);
			frameStr += ",";
		}
		for(int i = 0; i<bFrameVals.size(); i++)
		{
			frameStr += intToHex(bFrameVals[i]);
			if(i != bFrameVals.size()-1)
				frameStr += ",";
		}
		return frameStr;
	}

	// Converts int to a 2 digit Hex String
	// Variable Format: 0xFA  A is first digit F is second digit 
	string intToHex(int num)
	{
			if(num > 255)             // prevents unusable values
				num = 255;
			else if(num < 0)
				num = 0;
       
		   int firstNum = num%16;
		   int secondNum = num/16;
		   char firstChar,secondChar;
       
		   if(firstNum > 9)
				firstChar = 'A' + firstNum - 10;
		   else
				firstChar = '0' + firstNum; 
       
		   if(secondNum > 9)
				secondChar = 'A' + secondNum - 10;
		   else
				secondChar = '0' + secondNum;     
           
		   string hexStr;
		   hexStr +=secondChar;
		   hexStr += firstChar;
		   return hexStr;               
	}


	void setKey(char c)
	{
		myChar = c;
	}

	Image tagFrame;
	Image processed;
	char myChar;
	
	private:
	
	/* Checks if a Color matches a given Tag Color
	   there is a 10% margin of error allowed to still be a match */
	bool isColorMatch(Color& testColor, Color& tagColor)
	{
		int tenPct = .1*255;		// +-10% for possible video color error
		if(testColor.r <= tagColor.r+tenPct && testColor.r >= tagColor.r-tenPct)
			if(testColor.g <= tagColor.g+tenPct && testColor.g >= tagColor.g-tenPct)
				if(testColor.b <= tagColor.b+tenPct && testColor.b >= tagColor.b-tenPct)
						return true;
		
		return false;
	}

	/* Scans a frame to determine the width of an LED location */ 
	int getLEDWidth(Mat &frame, Color &tagColor, int iniX, int iniY)
	{
		tagFrame(frame);
		int WIDTH = tagFrame.getWidth();
		int HEIGHT = tagFrame.getHeight();

		int tagWidth = 0;
		bool scan = true;
		int x = iniX;
		int y = iniY;
		while(scan)
		{
			if( x+1 > WIDTH)	// Out of Bounds
				scan = false;
			else
			{
				Color currentColor = tagFrame.get(x,y);
				scan = isColorMatch(tagColor,currentColor);
				if(scan)
					x++;
			}
		}
		return x - iniX;
	}
	
	/* Scans a frame to determine the height of an LED location */ 
	int getLEDHeight(Mat &frame, Color &tagColor, int iniX, int iniY)
	{
		tagFrame(frame);
		int WIDTH = tagFrame.getWidth();
		int HEIGHT = tagFrame.getHeight();

		int tagWidth = 0;
		bool scan = true;
		int x = iniX;
		int y = iniY;
		while(scan)
		{
			if( y+1 > HEIGHT)	// Out of Bounds
				scan = false;
			else
			{
				Color currentColor = tagFrame.get(x,y);
				scan = isColorMatch(tagColor,currentColor);
				if(scan)
					y++;
			}
		}
		return y - iniY;
	}

};