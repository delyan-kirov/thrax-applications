# Applications

Programs written *in* Thrax, using the language as an SDK: the compiler, the
standard library, and the C FFI are the platform, and each subdirectory here is a
self-contained app built on top of it.

The intent is for each app to be its own repository, added here as a **git
submodule**, so an app has its own history and can be developed independently
while still sitting inside the Thrax tree for a batteries-included dev shell. (An
app can also just live here as a plain directory, as the launcher does today.)

Each app carries its own `flake.nix` providing whatever native libraries it binds
(raylib, etc.) plus the display/runtime deps, and a shell hook that links the
Thrax standard `library/` and any shared objects into place so the directory is
self-contained.

## Apps

- **[launcher](launcher/)** -- a raylib program launcher. The first app to use
  the C-struct FFI (`@struct @extern "C"`, structs passed to C by value).
- **[cbindgen](cbindgen/)** -- a C-header binding generator written in Thrax:
  reads a `.h`, emits Thrax `@extern` bindings for its enums, structs, unions, and
  functions.
- **[xml](xml/)** -- an XML parser, serializer and query engine built on
  algebraic effects: the cursor is a handler's parameter, an alternative is a
  `fork` the handler resumes twice (so backtracking rewinds the input by
  itself), and the same effect drives the query engine.
- **[web](web/)** -- the browser playground: the compiler itself built to
  `wasm32-unknown-unknown` and driven from hand-written JS, so every example on
  the page is compiled and run client-side. `site/playground.wasm` is checked in,
  so rebuild it with `node web/build.mjs` after any change to `crates/` or
  `library/`; otherwise the site keeps running whatever compiler it was last
  built from.

## Adding an app as a submodule

From the repo root:

```
git submodule add <repo-url> applications/<name>
```

Give the app a `flake.nix` (see `launcher/flake.nix`) and a `MAIN.thx` with an
entry point, `$ @main : @vec @str -> <@io> @int`. Run it with `thrax run MAIN.thx`
(interpreter) or `thrax build MAIN.thx` (native binary) from the app's own
directory.
