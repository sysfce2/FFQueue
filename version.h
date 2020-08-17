#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2020";
	static const char UBUNTU_VERSION_STYLE[] =  "20.08";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 7;
	static const long BUILD  = 54;
	static const long REVISION  = 267;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1755;
	#define RC_FILEVERSION 1,7,54,267
	#define RC_FILEVERSION_STRING "1, 7, 54, 267\0"
	static const char FULLVERSION_STRING [] = "1.7.54.267";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;
	

}
#endif //VERSION_H
