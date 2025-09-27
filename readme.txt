a C logging library

format specification

    Everythin in "{}" is treated as a format to be filled at logging time.
    Format can be specified as
        [{justing option|colors}[:...]]template option

    Where
        Template options:
            * $ - log message
            * n - logger's name
            * l - log level (long)
            * L - log level (short)
            * s - source file (like /src/main.c)
            * @ - basename source file (like main.c)
            * # - source line
            * f - function
            * T - logger tags separated by ':'
            * t - last added tag

    Justing options is as follows
        justing
            * < - just left
            * > - just right
            * ^ - just center

        truncating
            * [ - truncate left (keep right portion)
            * ] - truncate right (keep left portion)

        and a number of characters to just/truncate

        example "{>]6:@}"
            if template value is less than 6 characters
            it will be justed to the left and
                "hello" -> " hello"

            if template value is more than 6 characters
            it will be truncated from the right
                "hello1234" -> "hello1"

    Available color options
        * $BLACK
        * $RED
        ... todo


    example format looks as follows
    char fmt[] = "[{^7:l}] - {$BLACK:$DIM:}{>]6:@}:{<4:#}{$RESET:} {$}\n"
        "["    -  constant value
        "{"    -  start format template
        "^7"   -  just to center with minimum of 7 characters
        ":"    -  template option separtor
        "l"    -  log level (long)
        "}"    -  end format template
        " - "  -  constant value
        "{"    -  start format template
        ">]20" -  just to left and cut from right to 20 characters,
        ":"    -  template option separtor
        "@"    -  basename source file (like main.c)
        "}"    -  end format template
        ...
