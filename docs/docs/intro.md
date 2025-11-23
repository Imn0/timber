---
sidebar_position: 1
slug: /
---

# Timber

Welcome to **timber** - a C logging library.
<img src="/img/screenshot.png" alt="Screenshot" style={{width: '80%'}} />

Timber is a logging library made to be simple to use.
Source code available at [GitHub](https://github.com/Imn0/timber).
Additionally there are bindings for [C++](https://github.com/Imn0/timber-cpp) and for [Python](https://github.com/Imn0/timber-py)

## Example

```c
#include <tmb/tmb.h>

int main(void) {
    tmb_logger_t* logger = TMB_LOGGER("my logger",
                                      .log_level = TMB_LOG_LEVEL_DEBUG);

    TMB_LOGGER_ADD(
            logger,
            TMB_SINK_STDOUT(),
            TMB_FORMAT("{D} {d} {$BLACK:$DIM}{@}:{#}{$RESET} {$LEVEL:$}\n"));

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literally just %s", "printf");
    LOG_INFO(logger, "message %d", 3);
    LOG_DEBUG(logger, "fancy message %d", 3);

    tmb_logger_destroy(logger);
}

```
