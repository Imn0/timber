---
sidebar_position: 3
---
# Getting started

---

## Logging a message

To log a message use one of the following macros

```c
TMB_LOG_FATAL(logger_or_format, ...)
TMB_LOG_ERROR(logger_or_format, ...)
TMB_LOG_WARN(logger_or_format, ...)
TMB_LOG_INFO(logger_or_format, ...)
TMB_LOG_DEBUG(logger_or_format, ...)
TMB_LOG_TRACE(logger_or_format, ...)
TMB_LOG(log_level, logger_or_format, ...)
```

:::note
The `TMB` prefix on above macros is stripped by default, if that is not
desirable define `TMB_DONT_STRIP_PREFIX` before including the header or as a
compile option.
:::

Logging macros can be called
with a printf style format string.

```c
LOG_INFO("hello world! %d", 3);
TMB_LOG(TMB_LOG_LEVEL_INFO, "hello world! %d", 3);
```

---

## Creating a logger

To create a logger with a simple configuration, where each sink has its own formatter a `TMB_LOGGER_ADD` macro can be used.

First create a logger

```c
tmb_logger_t* my_logger = TMB_LOGGER("my logger");
```

In this state `my_logger` will use the library default format and the default sink.
Essentially behaving the same as the library default logger.  

```c
TMB_LOGGER_ADD(
        my_logger,
        TMB_SINK_STDOUT(),
        TMB_FORMAT("{$}\n"));
```

After this configuration any logging will look just like a normal printf.
To use this newly created logger the same logging macros can be called

```c
LOG_INFO(my_logger, "hello world! %d", 3);
TMB_LOG(TMB_LOG_LEVEL_INFO, my_logger, "hello world! %d", 3);
```

For detailed specification of logger message template string see the [Format Guide](./format.md).

[See the full example↗](https://github.com/Imn0/timber/blob/master/examples/02-simple_logger.c)

---

## Creating a sink

Timber provides several pre-made sinks with a convenient macro to create them.
Macros have a name in this format

```js
TMB_SINK_<sink name>(<specific sink options>, ...)
```

In the variadic macro argument common sink options can be passed,
right now it's the minimum log level that this sink will log.

For example this is how to create a file sink outputting to file `log.txt` and minimum logging level of `WARN`.

```c
tmb_sink_t* file_sink = TMB_SINK_FILE("log.txt", .min_log_level = TMB_LOG_LEVEL_WARNING)
```

:::note
min_log_level is an optional argument, with a default option of `TMB_LOG_LEVEL_DEBUG` (defined using `TMB_SINK_DEFAULT_OPTS`)
:::
