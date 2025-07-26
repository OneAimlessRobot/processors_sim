#ifndef HELPER_H
#define HELPER_H
#define COMMENT_STRING "\/\/"

int strings_are_equal(char* str1,char* str2);
void printWord(int fd,u_int32_t line);
void printWordNcurses(u_int32_t line);
int32_t firstBitOne(u_int32_t line);
int32_t lastBitOne(u_int32_t line);
void skip_cpu_comments(FILE* fp);
#endif
