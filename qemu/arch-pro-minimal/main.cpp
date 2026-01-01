#include <string.h>
#include <stdint.h>
#include <unistd.h>  // For `read()`
#include <iostream>  // For `std::cin`

#define MAX 10

char get_fuzz_input() {
    char c;
    if (read(0, &c, 1) != 1) return '\0';  // Read 1 byte from stdin
    return c;
}

int my_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

int main()
{
    int i = 0;
    char str[] = "password";
    char read_str[MAX] = {0};
    char read_c;

    do{
        read_c = get_fuzz_input();
        read_str[i] = read_c;
        i++;
        
    } while(read_c!='\n' && read_c!='\0' && i<MAX);

    read_str[i-1] = 0;
    
    if(!my_strcmp(read_str, str)) {
        abort();
    }else{
        return 0;
    }
}