#ifndef INCLUDE_UTILS_HH
#define INCLUDE_UTILS_HH

#define QUOTEME(x)								#x
#define CONCATENATE_PATH_EXPANDED(path,file)	QUOTEME(path##file)
#define CONCATENATE_PATH(directory,file)		CONCATENATE_PATH_EXPANDED(directory,file)

#define QUOTE_EXPANDED(x)						QUOTEME(x)
#define LOCATION_IN_SOURCE						__FILE__ "(" QUOTE_EXPANDED(__LINE__) ")"

#endif // INCLUDE_UTILS_HH
