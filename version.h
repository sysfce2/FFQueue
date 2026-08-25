#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{

	//Date Version Types
	static const char DATE[] = "14";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2026";
	static const char UBUNTU_VERSION_STYLE[] =  "26.04";

	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";

	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 7;
	static const long BUILD  = 71;
	static const long REVISION  = 353;

	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1858;
	#define RC_FILEVERSION 1,7,71,353
	#define RC_FILEVERSION_STRING "1, 7, 71, 353\0"
	static const char FULLVERSION_STRING [] = "1.7.71.353";

	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;

}
#endif //VERSION_H
