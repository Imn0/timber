# Format guide

Timber provides a implementation of the formatter interface based on a template string.
This formatter can be created with either a function

```c
tmb_formatter_t tmb_formatter_make(const char* fmt);
```

or a macro

```c
TMB_FORMAT(fmt)
```

Formatter then added to a logger using `tmb_logger_add_formatter`.
It can be also directly created and added to a logger with
`tmb_logger_add_format` and `tmb_logger_set_default_format` functions.

Timber uses a template string system similar to Python's f-strings for flexible log formatting.

## Basic Syntax

Everything inside `{}` is treated as a format template to be filled at logging time.

```plain
{[justifying_option|colors][:...]template_option}
```

## Template Options

| Option | Description |
|--------|-------------|
| `$` | Log message |
| `n` | Logger's name |
| `l` | Log level (long format, e.g., "INFO") |
| `L` | Log level (short format, e.g., "I") |
| `s` | Source file (full path, e.g., `/src/main.c`) |
| `@` | Basename of source file (e.g., `main.c`) |
| `#` | Source line number |
| `f` | Function name |
| `T` | Logger tags separated by ':' |
| `t` | Last added tag |
| `d` | Time since last log message on given logger |
| `D` | Log timestamp |

:::note
To output a `{` or `}` use `{{` `}}` respectively.
:::

## Justifying Options

### Alignment

- `<` - Justify left
- `>` - Justify right
- `^` - Justify center

### Truncation

- `[` - Truncate left (keep right portion)
- `]` - Truncate right (keep left portion)

### Width

A number specifying the minimum width for justification or maximum width for truncation.

### Combined Example

`{>]6:@}` - Justify right and truncate from right to 6 characters

- If value is less than 6 characters, it will be right-justified:
  - `"hello"` → `" hello"`
- If value is more than 6 characters, it will be truncated from the right:
  - `"hello1234"` → `"hello1"`

## Color Options

Colors are provided using [ansi escape codes↗](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797).  
Available color codes:

- `$BLACK`
- `$RED`
- `$GREEN`
- `$YELLOW`
- `$BLUE`
- `$MAGENTA`
- `$CYAN`
- `$WHITE`
- `$DIM`
- `$BOLD`
- `$RESET`
- `$LEVEL` - outputs color corresponding to message's log level

Colors can be chained using `:` separator.

:::note
Coloring options can be used either as a part of a template or as the template itself

```plain
{$RED:$}
```

is equivalent to

```plain
{$RED}{$}{$RESET}
```

:::

## Complete Example

```c
TMB_FORMAT("[{^7:l}] - {$BLACK:$DIM}{>]6:@}:{<4:#}{$RESET} {$}\n");
```

Breaking down this format string:

| Component | Description |
|-----------|-------------|
| `"["` | Constant value (literal bracket) |
| `{^7:l}` | Log level, center-justified to 7 characters |
| `" - "` | Constant value (separator) |
| `{$BLACK:$DIM:}` | Start black dim color |
| `{>]6:@}` | Basename of source file, right-justified/right-truncated to 6 chars |
| `":"` | Constant value (colon separator) |
| `{<4:#}` | Line number, left-justified to 4 characters |
| `{$RESET:}` | Reset colors |
| `" "` | Constant value (space) |
| `{$}` | The actual log message |
| `"\n"` | Constant value (newline) |
