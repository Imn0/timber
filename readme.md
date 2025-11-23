# Timber

## A C 23 logging library

<img src="./docs/static/img/screenshot.png" width="600" style="display:block; margin:auto;" />

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

## Docs

see docs at [timber.imno.ovh](https://timber.imno.ovh) or in [markdown](./docs/docs)
