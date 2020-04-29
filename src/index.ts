if (process.platform === 'win32')
    throw new Error('unsupported platform');

import {ITermios, INative} from './interfaces';
import * as path from 'path';
const native: INative = require(path.join('..', 'build', 'Release', 'termios.node'));
const s = native.ALL_SYMBOLS;

const EXPLAIN = native.EXPLAIN;
// we expect tcflag_t to be of width 4 (unsigned int), cc_t a single byte
if (EXPLAIN.members.c_iflag.width !== 4) throw new Error('unexpected c_iflag type');
if (EXPLAIN.members.c_oflag.width !== 4) throw new Error('unexpected c_oflag type');
if (EXPLAIN.members.c_cflag.width !== 4) throw new Error('unexpected c_cflag type');
if (EXPLAIN.members.c_lflag.width !== 4) throw new Error('unexpected c_lflag type');
if (EXPLAIN.members.c_cc.elem_size !== 1) throw new Error('unexpected cc_t type');

const ENDIANNESS = require('os').endianness();
const ACCESSORS = {
    BE: {
        2: [(b: Buffer, offset: number) => b.readUInt16BE(offset),
            (b: Buffer, value: number, offset: number) => b.writeUInt16BE(value, offset)],
        4: [(b: Buffer, offset: number) => b.readUInt32BE(offset),
            (b: Buffer, value: number, offset: number) => b.writeUInt32BE(value, offset)]
    },
    LE: {
        2: [(b: Buffer, offset: number) => b.readUInt16LE(offset),
            (b: Buffer, value: number, offset: number) => b.writeUInt16LE(value, offset)],
        4: [(b: Buffer, offset: number) => b.readUInt32LE(offset),
            (b: Buffer, value: number, offset: number) => b.writeUInt32LE(value, offset)]
    }
}

class Termios implements ITermios {
    private _data: Buffer;

    public get c_iflag(): number {
        return ACCESSORS[ENDIANNESS][EXPLAIN.members.c_iflag.width][0](this._data, EXPLAIN.members.c_iflag.offset);
    }
    public set c_iflag(value: number) {
        ACCESSORS[ENDIANNESS][EXPLAIN.members.c_iflag.width][1](this._data, value, EXPLAIN.members.c_iflag.offset);
    }

    public get c_oflag(): number {
        return ACCESSORS[ENDIANNESS][EXPLAIN.members.c_oflag.width][0](this._data, EXPLAIN.members.c_oflag.offset);
    }
    public set c_oflag(value: number) {
        ACCESSORS[ENDIANNESS][EXPLAIN.members.c_oflag.width][1](this._data, value, EXPLAIN.members.c_oflag.offset);
    }

    public get c_cflag(): number {
        return ACCESSORS[ENDIANNESS][EXPLAIN.members.c_cflag.width][0](this._data, EXPLAIN.members.c_cflag.offset);
    }
    public set c_cflag(value: number) {
        ACCESSORS[ENDIANNESS][EXPLAIN.members.c_cflag.width][1](this._data, value, EXPLAIN.members.c_cflag.offset);
    }

    public get c_lflag(): number {
        return ACCESSORS[ENDIANNESS][EXPLAIN.members.c_lflag.width][0](this._data, EXPLAIN.members.c_lflag.offset);
    }

    public set c_lflag(value: number) {
        ACCESSORS[ENDIANNESS][EXPLAIN.members.c_lflag.width][1](this._data, value, EXPLAIN.members.c_lflag.offset);
    }

    public get c_cc(): Buffer {
        const end = EXPLAIN.members.c_cc.offset + EXPLAIN.members.c_cc.width;
        return this._data.subarray(EXPLAIN.members.c_cc.offset, end);
    }

    constructor(from?: number | ITermios | null) {
        this._data = Buffer.from(Array(EXPLAIN.size));
        if (typeof from === 'number') {
            this.loadFrom(from);
        } else if (from instanceof Termios) {
            this._data = Buffer.from(from._data);
        } else if (from === undefined) {
            // TODO: load defaults from ttydefaults.h
        } else if (from !== null) {
            // null explicitly loads empty termios data, anything else throws an error
            throw new Error('unsupported fd value');
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

export {Termios, native};
