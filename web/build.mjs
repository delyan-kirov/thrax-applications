// Rebuild the playground wasm and verify it: the three steps that have to run
// together, in one command.
//
//   node web/build.mjs
//
// `site/playground.wasm` is a checked-in binary, so it does not follow the
// compiler the way the rest of the tree does. A stale one keeps working while
// silently shipping an older compiler, with whatever performance and syntax that
// compiler had. Rebuild after every change to crates/ or library/.
//
// Needs `wasm-ld` on PATH, which the web dev shell supplies (web/flake.nix).

import { spawnSync } from "node:child_process";
import { copyFileSync, statSync } from "node:fs";
import { fileURLToPath } from "node:url";

const web = fileURLToPath(new URL(".", import.meta.url));
const workspace = fileURLToPath(new URL("..", import.meta.url));
const built = `${workspace}target/wasm32-unknown-unknown/release/playground.wasm`;
const shipped = `${web}site/playground.wasm`;

function run(cmd, args, cwd) {
  console.log(`+ ${cmd} ${args.join(" ")}`);
  const { status, error } = spawnSync(cmd, args, { cwd, stdio: "inherit" });
  if (error?.code === "ENOENT") {
    console.error(`\n${cmd} is not on PATH. Enter the web dev shell first:\n` +
      `    nix develop ./web\n`);
    process.exit(1);
  }
  if (status !== 0) process.exit(status ?? 1);
}

run("cargo", ["build", "-p", "playground", "--target", "wasm32-unknown-unknown", "--release"], workspace);
copyFileSync(built, shipped);
console.log(`+ site/playground.wasm  (${(statSync(shipped).size / 1024).toFixed(0)} KiB)`);

// The smoke test runs every tour cell through the wasm just built, so a cell
// written against dead syntax cannot reach the site.
run("node", ["smoke.mjs"], web);
