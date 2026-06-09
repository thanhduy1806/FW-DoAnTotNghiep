#!/usr/bin/env python3
"""
xbld_sign.py — Firmware image signing tool for xBLD bootloader.

Creates a signed hex file with a 512-byte xBLD_ImageHeader_t prepended.
Compatible with xbld_image.h / xbld_image.c verification.

Header layout (512 bytes total):
  Offset  Size  Field
  0       4     magic          = 0xB007C0DE
  4       2     version_major
  6       2     version_minor
  8       2     version_patch
  10      2     hdr_version    = 1
  12      4     image_size     (firmware only, not including header)
  16      4     load_addr      (= hdr_addr + 512)
  20      4     entry_addr     (= load_addr for ARM)
  24      4     image_crc32    (zlib.crc32 of firmware bytes)
  28      4     hdr_crc32      (zlib.crc32 of header with this field = 0)
  32      4     timestamp      (unix time)
  36      4     build_number
  40      1     hw_id
  41      1     flags
  42      6     reserved
  48      464   padding (0x00)
  ----
  512 bytes total

CRC32: zlib.crc32 (ISO-HDLC, poly 0xEDB88320 reflected)

Usage:
  python xbld_sign.py input.hex output_signed.hex
  python xbld_sign.py input.hex output_signed.hex --hw-id 1 --hdr-addr 0x0040C000

  # With version file
  python xbld_sign.py input.hex output.hex --version-file version.txt
"""

import struct
import zlib
import time
import sys
import os
import argparse

try:
    import intelhex
except ImportError:
    print("Error: 'intelhex' package required. Install with: pip install intelhex")
    sys.exit(1)

# ── Constants ──────────────────────────────────────────────────────────────
IMAGE_MAGIC        = 0xB007C0DE
IMAGE_HEADER_SIZE  = 512
IMAGE_HDR_VERSION  = 1
MAX_FLASH_RANGE    = 2000 * 1024  # 2MB

HW_IDS = {
    'samv71':    1,
    'stm32f4':   2,
    'stm32h7':   3,
    'tiva_tm4c': 4,
}


def crc32(data: bytes) -> int:
    """CRC32 compatible with zlib / xBLD firmware."""
    return zlib.crc32(data) & 0xFFFFFFFF


# ── Version helpers ────────────────────────────────────────────────────────

def read_version(path: str) -> dict:
    ver = {'MAJOR': 1, 'MINOR': 0, 'PATCH': 0, 'BUILD': 0}
    try:
        if os.path.exists(path):
            with open(path, 'r') as f:
                for line in f:
                    line = line.strip()
                    if '=' in line and not line.startswith('#'):
                        k, v = line.split('=', 1)
                        k, v = k.strip(), v.strip()
                        if k in ver:
                            ver[k] = int(v)
    except Exception as e:
        print(f"  [WARN] Read version failed: {e}")
    return ver


def write_version(path: str, ver: dict):
    lines = []
    if os.path.exists(path):
        with open(path, 'r') as f:
            for line in f:
                s = line.strip()
                if '=' in s and not s.startswith('#'):
                    k = s.split('=', 1)[0].strip()
                    if k in ver:
                        line = f"{k}={ver[k]}\n"
                lines.append(line)
    else:
        for k, v in ver.items():
            lines.append(f"{k}={v}\n")
    with open(path, 'w') as f:
        f.writelines(lines)


# ── Build header ───────────────────────────────────────────────────────────

def build_header(ver: dict, image_size: int, load_addr: int,
                 image_crc: int, hw_id: int, flags: int = 0) -> bytes:
    """Build a 512-byte xBLD_ImageHeader_t."""
    hdr = bytearray(IMAGE_HEADER_SIZE)

    # Pack fields
    struct.pack_into('<I', hdr, 0,  IMAGE_MAGIC)
    struct.pack_into('<H', hdr, 4,  ver['MAJOR'])
    struct.pack_into('<H', hdr, 6,  ver['MINOR'])
    struct.pack_into('<H', hdr, 8,  ver['PATCH'])
    struct.pack_into('<H', hdr, 10, IMAGE_HDR_VERSION)
    struct.pack_into('<I', hdr, 12, image_size)
    struct.pack_into('<I', hdr, 16, load_addr)
    struct.pack_into('<I', hdr, 20, load_addr)       # entry_addr = load_addr (ARM)
    struct.pack_into('<I', hdr, 24, image_crc)
    struct.pack_into('<I', hdr, 28, 0)               # hdr_crc32 placeholder
    struct.pack_into('<I', hdr, 32, int(time.time()))
    struct.pack_into('<I', hdr, 36, ver.get('BUILD', 0))
    struct.pack_into('<B', hdr, 40, hw_id)
    struct.pack_into('<B', hdr, 41, flags)
    # bytes 42..47 = reserved (already 0)
    # bytes 48..511 = padding (already 0)

    # Compute header CRC with hdr_crc32 field = 0
    h_crc = crc32(bytes(hdr))
    struct.pack_into('<I', hdr, 28, h_crc)

    return bytes(hdr)


# ── Main signing logic ─────────────────────────────────────────────────────

def sign_hex(args):
    print("=" * 56)
    print("  xbld_sign.py — xBLD Firmware Signing Tool")
    print("=" * 56)

    # 1. Version
    ver = read_version(args.version_file)
    print(f"  Version: {ver['MAJOR']}.{ver['MINOR']}.{ver['PATCH']} "
          f"Build #{ver['BUILD']}")

    # 2. Load input hex
    print(f"  Loading: {args.input}")
    ih = intelhex.IntelHex(args.input)

    # 3. Layout
    hdr_addr   = args.hdr_addr
    load_addr  = hdr_addr + IMAGE_HEADER_SIZE
    flash_end  = hdr_addr + MAX_FLASH_RANGE

    # 4. Find image extent
    segments = ih.segments()
    actual_max = 0
    for s_start, s_end in segments:
        if s_start >= load_addr and s_end <= flash_end:
            actual_max = max(actual_max, s_end)

    if actual_max == 0:
        actual_max = ih.maxaddr() + 1

    image_size = actual_max - load_addr
    if image_size <= 0:
        print(f"  [ERROR] No valid image data found at 0x{load_addr:08X}")
        sys.exit(1)

    print(f"  Image size: {image_size:,} bytes "
          f"(0x{load_addr:08X} — 0x{actual_max:08X})")

    # 5. Extract image bytes and compute CRC
    image_bytes = bytes(ih.tobinarray(start=load_addr, size=image_size))
    img_crc = crc32(image_bytes)
    print(f"  Image CRC32: 0x{img_crc:08X}")

    # 6. Build header
    hw_id = args.hw_id
    if isinstance(hw_id, str):
        hw_id = HW_IDS.get(hw_id.lower(), 1)

    header = build_header(ver, image_size, load_addr, img_crc, hw_id)
    h_crc = struct.unpack_from('<I', header, 28)[0]
    print(f"  Header CRC32: 0x{h_crc:08X}")
    print(f"  HW ID: {hw_id}")

    # 7. Build output hex
    print("  Building output hex...")
    out_ih = intelhex.IntelHex()

    # Write header
    out_ih.frombytes(header, offset=hdr_addr)
    # Write image
    out_ih.frombytes(image_bytes, offset=load_addr)

    # Copy other valid segments (overlay: header/image take priority)
    for s_start, s_end in segments:
        if s_start >= hdr_addr and s_end <= flash_end:
            chunk = ih[s_start:s_end]
            out_ih.merge(chunk, overlap='ignore')

    # 8. Write output
    out_ih.write_hex_file(args.output)
    out_size = os.path.getsize(args.output)
    print(f"  Output: {args.output} ({out_size:,} bytes)")

    # 9. Update build number
    if not args.no_increment:
        ver['BUILD'] += 1
        write_version(args.version_file, ver)
        print(f"  Build number incremented -> {ver['BUILD']}")

    # 10. Generate C header (optional)
    if args.gen_header:
        ver_dir = os.path.dirname(os.path.abspath(args.version_file))
        h_path = os.path.join(ver_dir, 'app_version.h')
        with open(h_path, 'w') as f:
            f.write("#ifndef APP_VERSION_H\n#define APP_VERSION_H\n\n")
            f.write(f'#define APP_VERSION_MAJOR  {ver["MAJOR"]}\n')
            f.write(f'#define APP_VERSION_MINOR  {ver["MINOR"]}\n')
            f.write(f'#define APP_VERSION_PATCH  {ver["PATCH"]}\n')
            f.write(f'#define APP_VERSION_STR    '
                    f'"{ver["MAJOR"]}.{ver["MINOR"]}.{ver["PATCH"]}"\n')
            f.write(f'#define APP_BUILD_NUMBER   {ver["BUILD"] - 1}\n')
            f.write(f'\n#endif /* APP_VERSION_H */\n')
        print(f"  Generated: {h_path}")

    print("=" * 56)
    print("  DONE")
    print("=" * 56)


# ── CLI ────────────────────────────────────────────────────────────────────

def parse_args():
    p = argparse.ArgumentParser(
        description='xBLD image signing tool — prepend 512B header to hex file')
    p.add_argument('input', help='Input hex file (unsigned)')
    p.add_argument('output', help='Output hex file (signed)')
    p.add_argument('--version-file', default='version.txt',
                   help='Version file (default: version.txt)')
    p.add_argument('--hw-id', default=1,
                   help='Hardware ID: integer or name (samv71, stm32f4, stm32h7, tiva_tm4c)')
    p.add_argument('--hdr-addr', type=lambda x: int(x, 0), default=0x0040C000,
                   help='Image header address (default: 0x0040C000)')
    p.add_argument('--no-increment', action='store_true',
                   help='Do not increment build number')
    p.add_argument('--gen-header', action='store_true', default=True,
                   help='Generate app_version.h C header (default: on)')
    p.add_argument('--no-gen-header', action='store_false', dest='gen_header',
                   help='Do not generate app_version.h')
    return p.parse_args()


if __name__ == '__main__':
    try:
        sign_hex(parse_args())
    except Exception as e:
        print(f"\n  [ERROR] {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)