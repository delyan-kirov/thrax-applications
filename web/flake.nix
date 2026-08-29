{
  description = "Thrax web playground: the compiler built to wasm32-unknown-unknown, run in the browser";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      # Self-contained toolchain for the web deliverable, so the main compiler
      # flake does not have to carry it. The playground is a member of the
      # `applications` workspace and reaches the compiler crates by relative path,
      # so build it from the workspace root (the parent of this directory).
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          # rustc/cargo build the `playground` crate to wasm32-unknown-unknown
          # (nixpkgs rustc ships that target's std). `lld` provides the `wasm-ld`
          # linker named by ../.cargo/config.toml; `gcc` links the host build
          # scripts. `nodejs` runs the site (serve.mjs) and the headless smoke
          # test (smoke.mjs), which load `site/playground.wasm`.
          pkgs.rustc
          pkgs.cargo
          pkgs.lld
          pkgs.gcc
          pkgs.nodejs
          pkgs.git
        ];

        shellHook = ''
          echo "web playground shell. From the applications workspace root (..):"
          echo "    cargo build -p playground --target wasm32-unknown-unknown --release"
          echo "    cp target/wasm32-unknown-unknown/release/playground.wasm web/site/"
          echo "    node web/smoke.mjs      # headless test    node web/serve.mjs   # serve"
        '';
      };
    };
}
