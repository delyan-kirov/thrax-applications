{
  description = "Thrax program launcher -- a raylib app using the C-struct FFI";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  # The Thrax compiler, pinned from GitHub. `nix flake update thrax` bumps it.
  inputs.thrax.url = "github:delyan-kirov/Thrax";

  outputs =
    { self, nixpkgs, thrax }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      thraxc = thrax.packages.${system}.thrax;
      font = "${pkgs.jetbrains-mono}/share/fonts/truetype/JetBrainsMono-Regular.ttf";

      # Libraries the compiled binary loads: raylib (named by the `@extern`
      # paths) plus the GL/X stack raylib itself pulls in to open a window.
      graphicsLibs = [
        pkgs.raylib
        pkgs.libGL
        pkgs.libx11
        pkgs.libxcursor
        pkgs.libxi
        pkgs.libxinerama
        pkgs.libxrandr
      ];

      # Tools the launcher shells out to (popen/system): the desktop-file scan
      # (gawk), dedup/bounding (coreutils sort/timeout), the `?` web search
      # (xdg-open). Prepended to PATH so the user's own terminal and the apps it
      # launches still resolve from their environment.
      runtimeTools = pkgs.lib.makeBinPath [
        pkgs.gawk
        pkgs.coreutils
        pkgs.xdg-utils
      ];

      launcher = pkgs.stdenv.mkDerivation {
        pname = "thrax-launcher";
        version = "0.1.0";
        src = self;

        nativeBuildInputs = [
          thraxc
          pkgs.gcc
          pkgs.makeWrapper
        ];
        buildInputs = graphicsLibs;

        buildPhase = ''
          runHook preBuild
          export HOME=$TMPDIR

          # Recreate the shell-linked deps the dev shell sets up (both gitignored,
          # so not in `src`): the standard library from the pinned compiler, and
          # the raylib the `@extern "bin/libraylib.so"` paths name (`thrax build`
          # canonicalizes this symlink and bakes its store dir as an rpath, so the
          # binary finds raylib from anywhere).
          ln -sf ${thrax}/library library
          mkdir -p bin
          ln -sf ${pkgs.raylib}/lib/libraylib.so bin/libraylib.so

          # `loadFontEx "bin/font.ttf"` is read at runtime; bake the store path so
          # the binary needs no working directory.
          substituteInPlace MAIN.thx --replace-fail '"bin/font.ttf"' '"${font}"'

          thrax build MAIN.thx
          runHook postBuild
        '';

        installPhase = ''
          runHook preInstall
          install -Dm755 thrax-out/MAIN $out/libexec/thrax-launcher
          makeWrapper $out/libexec/thrax-launcher $out/bin/thrax-launcher \
            --prefix PATH : ${runtimeTools}
          runHook postInstall
        '';

        meta = {
          description = "A raylib program launcher written in Thrax";
          mainProgram = "thrax-launcher";
        };
      };
    in
    {
      packages.${system} = {
        inherit launcher;
        default = launcher;
      };

      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          pkgs.raylib # the library the launcher binds via @extern

          # Runtime deps of libraylib.so, so a window can open.
          pkgs.libGL
          pkgs.libx11
          pkgs.libxcursor
          pkgs.libxi
          pkgs.libxinerama
          pkgs.libxrandr

          # External tools the launcher shells out to (via popen/system):
          pkgs.gawk # the embedded desktop-file scan in `list_apps_cmd`
          pkgs.coreutils # sort (dedup the scan), timeout (bound `!` commands)
          pkgs.xdg-utils # xdg-open (the `?` web search)
        ];

        # Make this directory self-contained: `bin/libraylib.so` is the library
        # the `@extern` paths name (relative to here), and `library/` is the Thrax
        # standard library the interpreter resolves `CORE` from. The `thrax`
        # binary comes from the repo build (see README).
        shellHook = ''
          export RAYLIB=${pkgs.raylib}
          mkdir -p bin
          ln -sf ${pkgs.raylib}/lib/libraylib.so bin/libraylib.so
          ln -sf ../../library library
          # The UI font `loadFont "bin/font.ttf"` names, provided by nixpkgs so
          # the directory stays self-contained (like bin/libraylib.so).
          ln -sf ${pkgs.jetbrains-mono}/share/fonts/truetype/JetBrainsMono-Regular.ttf bin/font.ttf
          echo "launcher shell ready. Build thrax once in the repo, then:"
          echo "    thrax run MAIN.thx      # or: ../../target/debug/thrax run MAIN.thx"
        '';
      };
    };
}
