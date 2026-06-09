"""Robot Framework library for SAMV71 UART CLI testing.

Design goals:
- Robust serial open with retry
- Line/text based command exchange
- Detailed log capture for CI artifacts
- Minimal dependencies: only pyserial
"""
from __future__ import annotations

import os
import time
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Optional

import serial
from robot.api import logger
from robot.api.deco import keyword, library


@dataclass
class SerialConfig:
    port: str
    baudrate: int = 115200
    timeout_s: float = 1.0
    newline: str = "\r\n"
    open_retries: int = 5
    command_retries: int = 2
    retry_delay_s: float = 0.5
    log_dir: str = "logs"


@library(scope="GLOBAL", auto_keywords=False)
class SAMV71SerialLibrary:
    def __init__(self) -> None:
        self.ser: Optional[serial.Serial] = None
        self.cfg: Optional[SerialConfig] = None
        self.log_file: Optional[Path] = None

    def _ensure_log_file(self) -> None:
        log_dir = Path(self.cfg.log_dir if self.cfg else "logs")
        log_dir.mkdir(parents=True, exist_ok=True)
        if self.log_file is None:
            ts = datetime.now().strftime("%Y%m%d_%H%M%S")
            self.log_file = log_dir / f"samv71_serial_{ts}.log"

    def _write_log(self, direction: str, text: str) -> None:
        self._ensure_log_file()
        stamp = datetime.now().isoformat(timespec="milliseconds")
        with self.log_file.open("a", encoding="utf-8") as f:
            f.write(f"[{stamp}] {direction}: {text}\n")

    def _require_open(self) -> serial.Serial:
        if self.ser is None or not self.ser.is_open:
            raise RuntimeError("Serial port is not open")
        return self.ser

    @keyword("Open SAMV71 Connection")
    def open_connection(
        self,
        port: str,
        baudrate: int = 115200,
        timeout_s: float = 1.0,
        newline: str = "\\r\\n",
        open_retries: int = 5,
        command_retries: int = 2,
        retry_delay_s: float = 0.5,
        log_dir: str = "logs",
    ) -> None:
        newline = newline.encode("utf-8").decode("unicode_escape")
        self.cfg = SerialConfig(
            port=port,
            baudrate=int(baudrate),
            timeout_s=float(timeout_s),
            newline=newline,
            open_retries=int(open_retries),
            command_retries=int(command_retries),
            retry_delay_s=float(retry_delay_s),
            log_dir=log_dir,
        )
        self._ensure_log_file()

        last_exc: Optional[Exception] = None
        for attempt in range(1, self.cfg.open_retries + 1):
            try:
                self.ser = serial.Serial(
                    port=self.cfg.port,
                    baudrate=self.cfg.baudrate,
                    timeout=self.cfg.timeout_s,
                    write_timeout=self.cfg.timeout_s,
                )
                time.sleep(0.2)
                self.reset_input_buffer()
                logger.info(f"Opened serial port {self.cfg.port} on attempt {attempt}")
                self._write_log("INFO", f"opened {self.cfg.port} baud={self.cfg.baudrate}")
                return
            except Exception as exc:  # pragma: no cover
                last_exc = exc
                self._write_log("WARN", f"open attempt {attempt} failed: {exc}")
                time.sleep(self.cfg.retry_delay_s)
        raise RuntimeError(f"Failed to open serial port {port}: {last_exc}")

    @keyword("Close SAMV71 Connection")
    def close_connection(self) -> None:
        if self.ser and self.ser.is_open:
            self._write_log("INFO", "closing serial port")
            self.ser.close()
        self.ser = None

    @keyword("Reset Input Buffer")
    def reset_input_buffer(self) -> None:
        ser = self._require_open()
        ser.reset_input_buffer()

    @keyword("Read Available")
    def read_available(self) -> str:
        ser = self._require_open()
        time.sleep(0.05)
        count = ser.in_waiting
        data = ser.read(count) if count > 0 else b""
        text = data.decode(errors="replace")
        if text:
            self._write_log("RX", text.rstrip())
        return text

    @keyword("Write Raw")
    def write_raw(self, text: str) -> None:
        ser = self._require_open()
        payload = text.encode("utf-8")
        ser.write(payload)
        ser.flush()
        self._write_log("TX", text.rstrip())

    @keyword("Send Command")
    def send_command(self, command: str) -> None:
        assert self.cfg is not None
        self.write_raw(command + self.cfg.newline)

    @keyword("Read Until Contains")
    def read_until_contains(self, expected: str, timeout_s: float = 3.0) -> str:
        ser = self._require_open()
        deadline = time.time() + float(timeout_s)
        buf = ""
        while time.time() < deadline:
            chunk = ser.read(max(1, ser.in_waiting or 1)).decode(errors="replace")
            if chunk:
                buf += chunk
                self._write_log("RX", chunk.rstrip())
                if expected in buf:
                    return buf
        raise AssertionError(f"Timeout waiting for token '{expected}'. Buffer so far:\n{buf}")

    @keyword("Execute Command And Expect")
    def execute_command_and_expect(
        self,
        command: str,
        expected: str,
        timeout_s: float = 3.0,
        retries: Optional[int] = None,
    ) -> str:
        assert self.cfg is not None
        attempts = int(retries) if retries is not None else self.cfg.command_retries + 1
        last_error: Optional[Exception] = None
        for attempt in range(1, attempts + 1):
            try:
                self.reset_input_buffer()
                self.send_command(command)
                response = self.read_until_contains(expected=expected, timeout_s=float(timeout_s))
                logger.info(f"Command '{command}' succeeded on attempt {attempt}")
                return response
            except Exception as exc:
                last_error = exc
                self._write_log("WARN", f"command '{command}' attempt {attempt} failed: {exc}")
                time.sleep(self.cfg.retry_delay_s)
        raise AssertionError(f"Command '{command}' failed after {attempts} attempts: {last_error}")

    @keyword("Wait For Boot Banner")
    def wait_for_boot_banner(self, expected: str = "SAMV71", timeout_s: float = 10.0) -> str:
        return self.read_until_contains(expected=expected, timeout_s=float(timeout_s))

    @keyword("Get Session Log Path")
    def get_session_log_path(self) -> str:
        self._ensure_log_file()
        return str(self.log_file)