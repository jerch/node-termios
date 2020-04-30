if (process.platform === 'win32')
    throw new Error('unsupported platform');

import {ITermios, INative} from './interfaces';
import * as path from 'path';
import { endianness, platform } from 'os';
export const native: INative = require(path.join('..', 'build', 'Release', 'termios.node'));
const s = native.ALL_SYMBOLS;

const ENDIAN = endianness();

/**
 * EXPLAIN holds the termios structure memory layout from C,
 * so we know how to access struct members.
 *
 * We have to explicitly test the type widths, since they are not fixed typed.
 * Still `tcflag_t`, typed as `unsigned int` on most platforms (except OSX),
 * translates to a 4 byte endian-aware read/write at offset.
 * We ignore the type being 2 bytes on older platforms, as they are not relevant.
 *
 * OSX/Darwin switched to a `unsigned long long` type (8 bytes), but only uses the
 * lower 32 bits. Thus we can treat it as 4 byte at offset on LE. BE would need
 * an offset shift by 4 bytes (currently not implemented).
 * Once they decide to use higher bits, we have to rework the whole symbol export
 * to use BigInt.
 * On C side the macro _SAFE32_ ensures, that a wider symbol does not lose bits by accident.
 *
 * Tested on: Linux 5, OSX Mojave, FreeBSD 11, OpenIndiana 2019.
 */
const EXPLAIN = native.EXPLAIN;
const T_SIZE = EXPLAIN.size;
const T_MEM = EXPLAIN.members;

// monkey patch width to 4 on darwin LE to pass the tests below
if (platform() === 'darwin' && ENDIAN === 'LE') {
    T_MEM.c_iflag.width = 4;
    T_MEM.c_oflag.width = 4;
    T_MEM.c_cflag.width = 4;
    T_MEM.c_lflag.width = 4;
}

// refuse any platform, that does not hold flags in 4 bytes / control codes in 1 byte
if (T_MEM.c_iflag.width !== 4) throw new Error('unexpected c_iflag type');
if (T_MEM.c_oflag.width !== 4) throw new Error('unexpected c_oflag type');
if (T_MEM.c_cflag.width !== 4) throw new Error('unexpected c_cflag type');
if (T_MEM.c_lflag.width !== 4) throw new Error('unexpected c_lflag type');
if (T_MEM.c_cc.elem_size !== 1) throw new Error('unexpected cc_t type');

// Handles BE/LE 4 byte access.
const ACCESSORS = {
    BE: {
        4: {
            read: (b: Buffer, offset: number) => b.readUInt32BE(offset),
            write: (b: Buffer, value: number, offset: number) => b.writeUInt32BE(value, offset)
        }
    },
    LE: {
        4: {
            read: (b: Buffer, offset: number) => b.readUInt32LE(offset),
            write: (b: Buffer, value: number, offset: number) => b.writeUInt32LE(value, offset)
        }
    }
}

export class Termios implements ITermios {
    private _data: Buffer;

    public get c_iflag(): number {
        return ACCESSORS[ENDIAN][T_MEM.c_iflag.width].read(this._data, T_MEM.c_iflag.offset);
    }
    public set c_iflag(value: number) {
        ACCESSORS[ENDIAN][T_MEM.c_iflag.width].write(this._data, value, T_MEM.c_iflag.offset);
    }

    public get c_oflag(): number {
        return ACCESSORS[ENDIAN][T_MEM.c_oflag.width].read(this._data, T_MEM.c_oflag.offset);
    }
    public set c_oflag(value: number) {
        ACCESSORS[ENDIAN][T_MEM.c_oflag.width].write(this._data, value, T_MEM.c_oflag.offset);
    }

    public get c_cflag(): number {
        return ACCESSORS[ENDIAN][T_MEM.c_cflag.width].read(this._data, T_MEM.c_cflag.offset);
    }
    public set c_cflag(value: number) {
        ACCESSORS[ENDIAN][T_MEM.c_cflag.width].write(this._data, value, T_MEM.c_cflag.offset);
    }

    public get c_lflag(): number {
        return ACCESSORS[ENDIAN][T_MEM.c_lflag.width].read(this._data, T_MEM.c_lflag.offset);
    }
    public set c_lflag(value: number) {
        ACCESSORS[ENDIAN][T_MEM.c_lflag.width].write(this._data, value, T_MEM.c_lflag.offset);
    }

    public get c_cc(): Buffer {
        return this._data.subarray(T_MEM.c_cc.offset, T_MEM.c_cc.offset + T_MEM.c_cc.width);
    }

    constructor(from?: number | ITermios | null) {
        this._data = Buffer.from(Array(T_SIZE));
        if (typeof from === 'number') {
            this.loadFrom(from);
        } else if (from instanceof Termios) {
            this._data = Buffer.from(from._data);
        } else if (from === undefined) {
            // TODO: load defaults from ttydefaults.h
        } else if (from !== null) {
            // null explicitly loads empty termios data, anything else throws an error
            throw new Error('unsupported from value');
        }
        // make termios primitives enumerable
        for(const property of ['c_iflag', 'c_oflag', 'c_cflag', 'c_lflag', 'c_cc']) {
			const desc = Object.getOwnPropertyDescriptor(Termios.prototype, property);
			const modified = Object.assign(desc, {enumerable: true});
			Object.defineProperty(this, property, modified);
		}
    }
    public loadFrom(fd: number): void {
        native.tcgetattr(fd, this._data);
    }
    public writeTo(fd: number, action?: number): void {
        if (typeof action === 'undefined')
            action = s.TCSAFLUSH;
        native.tcsetattr(fd, action, this._data);
    }
    public getInputSpeed(): number {
        return native.cfgetispeed(this._data);
    }
    public getOutputSpeed(): number {
        return native.cfgetospeed(this._data);
    }
    public setInputSpeed(baudrate: number): void {
        native.cfsetispeed(this._data, baudrate);
    }
    public setOutputSpeed(baudrate: number): void {
        native.cfsetospeed(this._data, baudrate);
    }
    public setSpeed(baudrate: number): void {
        native.cfsetispeed(this._data, baudrate);
        native.cfsetospeed(this._data, baudrate);
    }
    public setraw(): void {
        this.c_iflag &= ~(s.BRKINT | s.ICRNL | s.INPCK | s.ISTRIP | s.IXON);
        this.c_oflag &= ~s.OPOST;
        this.c_cflag &= ~(s.CSIZE | s.PARENB);
        this.c_cflag |= s.CS8;
        this.c_lflag &= ~(s.ECHO | s.ICANON | s.IEXTEN | s.ISIG);
        this.c_cc[s.VMIN] = 1;
        this.c_cc[s.VTIME] = 0;
    }
    public setcbreak(): void {
        this.c_lflag &= ~(s.ECHO | s.ICANON);
        this.c_cc[s.VMIN] = 1;
        this.c_cc[s.VTIME] = 0;
    }
    public setcooked(): void {
        this.c_iflag = s.BRKINT | s.ICRNL | s.INPCK | s.ISTRIP | s.IXON | s.IGNPAR;
        this.c_oflag = s.OPOST | s.ONLCR;
        this.c_cflag |= s.CS8;
        this.c_lflag = s.ECHOKE | s.ECHOCTL | s.ECHOK | s.ECHOE | s.ECHO | s.ICANON | s.IEXTEN | s.ISIG;
        // FIXME: set c_cc values;
    }
}
