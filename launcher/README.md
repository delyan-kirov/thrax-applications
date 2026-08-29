# Thrax Launcher

A tiny program launcher written in Thrax against [raylib](https://www.raylib.com/).
Type to filter the list, then click a button to spawn the program it names, or
lead the query with a sigil to run a command, search the web, or invoke a tool.

The whole raylib API is imported from `RAYLIB.thx`, which is **generated** by the
cbindgen tool (`applications/cbindgen`) from raylib's header. `MAIN.thx` just does
`$ with RAYLIB` and calls the bindings. It showcases Thrax's **C-struct FFI**:
raylib's `Color` is a real `@struct @extern "C"` passed **by value**, built with
an ordinary struct literal.

A C function is never curried, so a raylib call with several parameters takes a
**record** of its arguments (fields named `p0`, `p1`, ... in C order); a
one-parameter call stays `A -> B`, and a `void`-taking call takes unit:

```
$ with RAYLIB
...
clearBackground (Color.{ .r = 24, .g = 24, .b = 32, .a = 255 })      # one arg
drawRectangle { .p0 = x, .p1 = y, .p2 = w, .p3 = h, .p4 = col }      # a record
beginDrawing {}                                                       # unit
```

The generated bindings use the `@`-sigil built-in types (`@int32`, `@nat8`, ...),
so coordinates are `@int32`; the app stays integer-only (positions, sizes, the
mouse via `getMouseX`/`getMouseY`), no floats.

## Type-to-search

The search box reads keystrokes with raylib's `getCharPressed` and filters the
list by a case-insensitive substring match. `getCharPressed` returns a C `int`
(`@int32`), but the string library works in `Int`, and those widths are distinct
types. The `@cast` intrinsic bridges them:

```
$ read_chars : Str -> Str = \q =
	let c = getCharPressed {} in            # c : @int32
	if c ?= 0 => q
	else if c >= 32 && c ?< 127 => read_chars (q ++ from_byte (@cast c))
	else read_chars q                       # from_byte wants Int; @cast c widens it
```

`@cast` reinterprets an integer at another width. It is type-directed: the target
comes from the checking context (an argument position or an annotated binding),
so `from_byte (@cast c)` casts to `Int` because `from_byte : Int -> Str`. Both
engines box integers uniformly, so it is a no-op at runtime; the actual C width is
applied only at the `@extern` boundary.

## Command modes

The first byte of the query picks a mode; press **Enter** to commit it. With no
sigil the query just filters the app list (unchanged), and apps still launch on
click.

| Prefix | Example | Effect |
| ------ | ------------- | ---------------------------------------------------- |
| `!`    | `! htop`      | Spawn a terminal running the command, then close the launcher. |
| `?`    | `? raylib`    | Open the query as a web search; the box clears, the launcher stays. |
| `$`    | `$time`       | Run a curated tool in a terminal; the box clears. |

The `$` tools are a small table in `MAIN.thx`: `time` runs `date`, `cal` runs
`cal`, `disk` runs `df -h`, `mem` runs `free -h`. An unknown name leaves the
query in place so the typo stays visible. Every mode is `system(3)` underneath,
so the terminal and search engine are two editable constants at the top of the
file:

```
$ terminal   : Str = "xterm"
$ search_url : Str = "https://duckduckgo.com/?q="
```

`!` and `$` open `terminal -e sh -c '<cmd>; exec $SHELL'`, so the window stays
open after the command exits. `?` hands the query to `xdg-open`, escaping spaces
as `+`.

Regenerate the bindings after a raylib upgrade:

```
LIB=bin/libraylib.so MOD=RAYLIB OUT=RAYLIB.thx \
  thrax run ../cbindgen/MAIN.thx <path-to>/raylib.h
```

## Run it

From the repo, build the `thrax` binary once:

```
nix develop -c cargo build -p thrax     # produces target/debug/thrax
```

Then, from **this directory**:

```
nix develop            # links bin/libraylib.so and library/ into place
thrax run MAIN.thx     # or: ../../target/debug/thrax run MAIN.thx
```

`nix develop`'s shell hook symlinks `bin/libraylib.so` (the library the
`@extern` paths name) and `library/` (the Thrax standard library the interpreter
resolves `CORE` from), so this directory is self-contained.

### Build a native binary instead

The C backend emits a real `typedef struct { ... } Color;` and passes it by
value, so the compiled program uses the platform ABI directly:

```
thrax build MAIN.thx   # emits MAIN.c, compiles and links -> ./MAIN
./MAIN
```

## Edit the app list

The launchable programs are a plain list of records near the top of `MAIN.thx`:

```
$ App : @struct = label: Str, cmd: Str,
$ apps : @list App =
    [ App.{ .label = "Terminal", .cmd = "xterm &" }
    , App.{ .label = "Files",    .cmd = "xdg-open . &" }
    , App.{ .label = "Browser",  .cmd = "firefox &" } ]
```

Each `cmd` is handed to `system(3)`; the trailing `&` spawns it in the
background so the launcher stays responsive.
