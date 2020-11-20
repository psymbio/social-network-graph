#include <string.h>
#include <stdlib.h>

char *incr(char *s)
{
    int i, begin, tail, len;
    int neg = (*s == '-');
    char tgt = neg ? '0' : '9';
    if (!strcmp(s, "-1"))
    {
        s[0] = '0', s[1] = '\0';
        return s;
    }

    len = strlen(s);
    begin = (*s == '-' || *s == '+') ? 1 : 0;
    for (tail = len - 1; tail >= begin && s[tail] == tgt; tail--)
        ;

    if (tail < begin && !neg)
    {
        if (!begin)
            s = realloc(s, len + 2);
        s[0] = '1';
        for (i = 1; i <= len - begin; i++)
            s[i] = '0';
        s[len + 1] = '\0';
    }
    else if (tail == begin && neg && s[1] == '1')
    {
        for (i = 1; i < len - begin; i++)
            s[i] = '9';
        s[len - 1] = '\0';
    }
    else
    {
        for (i = len - 1; i > tail; i--)
            s[i] = neg ? '9' : '0';
        s[tail] += neg ? -1 : 1;
    }

    return s;
}

char *string_test(const char *s)
{
    char *ret = malloc(strlen(s));
    strcpy(ret, s);
    return incr(ret);
}
