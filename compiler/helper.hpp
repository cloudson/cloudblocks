#ifndef CLOUD_HELPER
#define CLOUD_HELPER 1

class Helper
{
	public:
		static char* str_cpy(const char* needle);
		static char* str_cat(const char* string1, const char* string2);
		static bool str_eq(const char* string1, const char* string2);
};

#endif