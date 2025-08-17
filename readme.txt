a C logging library

format specification
    * %m - log message
    * %n - loggers name
    * %l - log level (long)
    * %L - log level (short)
    * %s - basename source file (like main.c)
    * %@ - source file (like /src/main.c)
    * %# - source line
    * %f - function
    * %t - logger tags

justing
    * n< - just left to n characters
    * n> - just right to n characters
    * n^ - just center to n characters

truncating
    * n[ - truncate from the left to n characters
    * n] - truncate from the right to n characters
