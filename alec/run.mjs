#!/usr/bin/env node
// Runs a module alec produced: node run.mjs prog.wasm [args...]
//
// The module imports its system interface from `env`: a handful of calls the C
// library is built on (write, read, exit, the arguments, files, the clock) and
// the libm functions wasm has no instruction for. Everything else an `env`
// import names is a C function the program declared but nobody defined, which
// is reported when it is first called.
//
// On wasm64 every pointer and size_t crosses the boundary as a BigInt; the
// helpers below normalize that, so each host function is written once.

import fs from "node:fs";

const [, , file, ...rest] = process.argv;
if (!file) {
	console.error("usage: node run.mjs prog.wasm [args...]");
	process.exit(2);
}

const bytes = fs.readFileSync(file);
const mem64 = memoryIs64(bytes);
const argv = [file, ...rest];

// A pointer or size argument, as a Number.
const P = (x) => Number(x);
// A `long`/`size_t` result: i64 on wasm64, i32 on wasm32.
const L = (x) => (mem64 ? BigInt(x) : x | 0);
// A `long long` result: always i64.
const LL = (x) => BigInt(Math.trunc(x));

class Exit {
	constructor(code) {
		this.code = code;
	}
}

let memory;
const view = () => new Uint8Array(memory.buffer);
const cstr = (p) => {
	const m = view();
	let e = P(p);
	while (m[e]) e++;
	return Buffer.from(m.subarray(P(p), e)).toString("utf8");
};
const encoded = argv.map((a) => Buffer.from(a + "\0", "utf8"));

const fds = new Map([[0, 0], [1, 1], [2, 2]]);
let nextFd = 3;

const host = {
	__alec_write(fd, buf, len) {
		const data = view().subarray(P(buf), P(buf) + P(len));
		try {
			return L(fs.writeSync(fds.get(fd) ?? fd, data));
		} catch {
			return L(-1);
		}
	},
	__alec_read(fd, buf, len) {
		const data = view().subarray(P(buf), P(buf) + P(len));
		try {
			return L(fs.readSync(fds.get(fd) ?? fd, data, 0, data.length, null));
		} catch (e) {
			return L(e.code === "EOF" ? 0 : -1);
		}
	},
	__alec_exit(code) {
		throw new Exit(code);
	},
	__alec_argc() {
		return argv.length;
	},
	__alec_arglen(i) {
		return L(encoded[i].length - 1);
	},
	__alec_argcopy(i, dst) {
		view().set(encoded[i], P(dst));
	},
	// `mode` is an fopen mode string ("r", "w+", "ab", ...); node takes the same.
	__alec_open(path, mode) {
		try {
			const fd = fs.openSync(cstr(path), cstr(mode).replace("b", ""));
			fds.set(nextFd, fd);
			return nextFd++;
		} catch {
			return -1;
		}
	},
	__alec_close(fd) {
		const real = fds.get(fd);
		if (real === undefined) return -1;
		fds.delete(fd);
		if (fd > 2) fs.closeSync(real);
		return 0;
	},
	__alec_filesize(fd) {
		try {
			return LL(fs.fstatSync(fds.get(fd)).size);
		} catch {
			return LL(-1);
		}
	},
	__alec_pread(fd, buf, len, off) {
		const data = view().subarray(P(buf), P(buf) + P(len));
		try {
			return L(fs.readSync(fds.get(fd), data, 0, data.length, Number(off)));
		} catch {
			return L(-1);
		}
	},
	__alec_remove(path) {
		try {
			fs.unlinkSync(cstr(path));
			return 0;
		} catch {
			return -1;
		}
	},
	__alec_time() {
		return LL(Date.now() / 1000);
	},
	__alec_clock() {
		return LL(performance.now() * 1000);
	},
	__alec_getenv_len(name) {
		const v = process.env[cstr(name)];
		return v === undefined ? -1 : Buffer.byteLength(v);
	},
	__alec_getenv_copy(name, dst) {
		const v = process.env[cstr(name)] ?? "";
		view().set(Buffer.from(v + "\0", "utf8"), P(dst));
	},
};

for (const f of ["sin", "cos", "tan", "asin", "acos", "atan", "exp", "log", "log10", "log2",
	"sinh", "cosh", "tanh", "asinh", "acosh", "atanh", "cbrt", "expm1", "log1p", "exp2"]) {
	host[f] = Math[f] ?? ((x) => 2 ** x);
}
host.atan2 = Math.atan2;
host.pow = Math.pow;
host.hypot = Math.hypot;
host.fmod = (a, b) => a % b;

const mod = new WebAssembly.Module(bytes);
const env = {};
for (const imp of WebAssembly.Module.imports(mod)) {
	if (imp.module !== "env") continue;
	env[imp.name] = host[imp.name] ?? (() => {
		throw new Error(`call to '${imp.name}', which is declared but not defined anywhere`);
	});
}

const inst = new WebAssembly.Instance(mod, { env });
memory = inst.exports.memory;
const entry = inst.exports._start ?? inst.exports.main;
let status = 0;
try {
	const r = entry();
	status = typeof r === "bigint" ? Number(r) : r ?? 0;
} catch (e) {
	if (e instanceof Exit) status = e.code;
	else {
		console.error(`alec: ${e instanceof WebAssembly.RuntimeError ? "trap" : "error"}: ${e.message}`);
		if (process.env.ALEC_TRACE) console.error(e.stack);
		status = 134;
	}
}
process.exit(status & 255);

// The memory section's limits flag has bit 2 set for memory64.
function memoryIs64(b) {
	let i = 8;
	const leb = () => {
		let r = 0, s = 0, x;
		do {
			x = b[i++];
			r |= (x & 127) << s;
			s += 7;
		} while (x & 128);
		return r >>> 0;
	};
	while (i < b.length) {
		const id = b[i++];
		const size = leb();
		if (id === 5) {
			leb();
			return (b[i] & 4) !== 0;
		}
		i += size;
	}
	return false;
}
