"""Evidence gates: wrong-ROM execution and false restoration success must fail."""
import base64
import argparse
import contextlib
import hashlib
import io
import json
import pathlib
import struct
import sys
import tempfile
import unittest
from unittest import mock
import zlib

import battle_probe as bp


def png_chunk(kind, data):
    return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)


def png_fixture(*, pixels=None, compressed=None, split=False):
    # Independently constructed, noninterlaced 256x240 RGB, filter 0 per row.
    pixels = (b'\0' + b'\x12\x34\x56' * 256) * 240 if pixels is None else pixels
    compressed = zlib.compress(pixels) if compressed is None else compressed
    header = b'\x89PNG\r\n\x1a\n' + png_chunk(b'IHDR', struct.pack('>IIBBBBB', 256, 240, 8, 2, 0, 0, 0))
    pieces = [compressed[:len(compressed)//2], compressed[len(compressed)//2:]] if split else [compressed]
    return header + b''.join(png_chunk(b'IDAT', part) for part in pieces) + png_chunk(b'IEND', b'')


class ProbeGates(unittest.TestCase):
    def test_empty_native_screenshot_is_not_evidence(self):
        with self.assertRaisesRegex(ValueError, 'PNG'):
            bp.check_png(b'')

    def test_png_header_without_image_is_not_evidence(self):
        with self.assertRaisesRegex(ValueError, 'PNG'):
            bp.check_png(png_fixture()[:33])

    def test_png_truncation_crc_and_trailing_bytes_are_rejected(self):
        good = png_fixture()
        bad_crc = bytearray(good)
        bad_crc[-1] ^= 1
        cases = {'truncated_chunk': good[:-3], 'bad_crc': bytes(bad_crc), 'trailing_bytes': good + b'x'}
        for name, raw in cases.items():
            with self.subTest(name=name), self.assertRaisesRegex(ValueError, 'PNG'):
                bp.check_png(raw)

    def test_png_compressed_data_must_be_complete_and_bounded(self):
        pixels = (b'\0' + b'\x12\x34\x56' * 256) * 240
        cases = {
            'empty_IDAT': b'',
            'invalid_zlib': b'not compressed image data',
            'truncated_zlib': zlib.compress(pixels)[:-2],
            'missing_scanline': zlib.compress(pixels[:-769]),
            'extra_scanline': zlib.compress(pixels + pixels[:769]),
            'second_zlib_stream': zlib.compress(pixels) + zlib.compress(b'extra'),
            'invalid_row_filter': zlib.compress(b'\x05' + pixels[1:]),
        }
        for name, compressed in cases.items():
            with self.subTest(name=name), self.assertRaisesRegex(ValueError, 'PNG'):
                bp.check_png(png_fixture(compressed=compressed))

    def test_png_chunk_order_and_critical_chunks_are_validated(self):
        good = png_fixture()
        header, image, end = good[:33], good[33:-12], good[-12:]
        cases = {
            'missing_IDAT': header + end,
            'missing_IEND': header + image,
            'duplicate_IHDR': header + header[8:] + image + end,
            'unknown_critical_chunk': header + png_chunk(b'ABCD', b'') + image + end,
            'noncontiguous_IDAT': header + image + png_chunk(b'tEXt', b'a\0b') + image + end,
        }
        for name, raw in cases.items():
            with self.subTest(name=name), self.assertRaisesRegex(ValueError, 'PNG'):
                bp.check_png(raw)

    def test_complete_native_rgb_png_and_split_idat_are_accepted(self):
        self.assertIsNone(bp.check_png(png_fixture()))
        self.assertIsNone(bp.check_png(png_fixture(split=True)))

    def test_wrong_rom_rejected_before_output_creation(self):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d)
            rom = p / 'wrong.nes'
            rom.write_bytes(b'not the authorized game')
            with self.assertRaisesRegex(ValueError, 'ROM'):
                bp.preflight(rom, p / 'out', None)
            self.assertFalse((p / 'out').exists())

    def test_nonempty_output_refused_without_removing_evidence(self):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d)
            keep = p / 'original.log'
            keep.write_text('keep this failure')
            with self.assertRaises(FileExistsError):
                bp.prepare_output(p)
            self.assertEqual(keep.read_text(), 'keep this failure')

    def test_empty_or_invalid_checkpoint_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d) / 'state.b64'
            for data in ('', 'not base64', base64.b64encode(b'bad').decode()):
                p.write_text(data)
                with self.assertRaises(ValueError):
                    bp.read_checkpoint(p)

    def test_pending_restore_cannot_be_reported_success(self):
        with self.assertRaisesRegex(ValueError, 'restor'):
            bp.check_completion('STATE pending\nDONE 20\n', 0, True, 20)

    def test_early_exit_cannot_be_reported_success(self):
        with self.assertRaisesRegex(ValueError, 'DONE'):
            bp.check_completion('LOADED\nDONE 19\n', 0, True, 20)

    def test_error_exit_cannot_be_reported_success(self):
        with self.assertRaisesRegex(ValueError, 'exit'):
            bp.check_completion('LOADED\nDONE 20\n', 1, True, 20)

    def test_valid_completion_receipt_accepted(self):
        self.assertIsNone(bp.check_completion('LOADED\nDONE 20\n', 0, True, 20))


class RunnerIdentityGates(unittest.TestCase):
    def fixture(self, directory, replace_executable=False):
        # A real bounded subprocess exercises runner IO; this is not Mesen.
        p = pathlib.Path(directory)
        rom, mesen, script = p / 'fixture.nes', p / 'fixture_mesen', p / 'fixture.lua'
        rom.write_bytes(b'\0' * 524304)
        script.write_text('-- synthetic identity gate fixture\n')
        core = b'synthetic core identity fixture'
        source = (f'#!{sys.executable}\nfrom pathlib import Path\n'
                  'Path("launched").write_text("synthetic subprocess")\n'
                  f'Path(__file__).with_name("MesenCore.so").write_bytes({core!r})\n')
        if replace_executable:
            source += ('replacement = Path(__file__).with_suffix(".replacement")\n'
                       'replacement.write_bytes(b"changed copied executable")\n'
                       'replacement.replace(__file__)\n')
        mesen.write_text(source)
        mesen.chmod(0o755)
        expected = {'ROM_SHA256': hashlib.sha256(rom.read_bytes()).hexdigest(),
                    'MESEN_SHA256': hashlib.sha256(mesen.read_bytes()).hexdigest(),
                    'CORE_SHA256': hashlib.sha256(core).hexdigest()}
        args = argparse.Namespace(rom=rom, mesen=mesen, script=script,
                                  output=p / 'run', emulator_timeout=3)
        return args, expected

    def test_copied_executable_corruption_is_rejected_before_launch(self):
        with tempfile.TemporaryDirectory() as directory:
            args, expected = self.fixture(directory)
            original_copy = bp.run_mesen.shutil.copy2

            def corrupt_copy(source, destination):
                result = original_copy(source, destination)
                with pathlib.Path(destination).open('ab') as stream:
                    stream.write(b'\n# corrupt copied image\n')
                return result

            with mock.patch.multiple(bp.run_mesen, **expected), \
                    mock.patch.object(bp.run_mesen.shutil, 'copy2', side_effect=corrupt_copy), \
                    contextlib.redirect_stdout(io.StringIO()):
                with self.assertRaisesRegex(ValueError, 'Copied.*SHA-256'):
                    bp.run_mesen.run(args)
            self.assertFalse((args.output / 'launched').exists())

    def test_replaced_executable_after_zero_exit_cannot_pass_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            args, expected = self.fixture(directory, replace_executable=True)
            with mock.patch.multiple(bp.run_mesen, **expected), contextlib.redirect_stdout(io.StringIO()):
                result = bp.run_mesen.run(args)
            receipt = json.loads((args.output / 'receipt.json').read_text())
            self.assertEqual(receipt['exit_code'], 0)
            self.assertEqual(receipt['core']['sha256'], expected['CORE_SHA256'])
            self.assertNotEqual(result, 0)
            self.assertFalse(receipt['runtime_identity_passed'])
            self.assertFalse(receipt['copied_executable_unchanged'])
            self.assertNotEqual(receipt['mesen_after']['sha256'], expected['MESEN_SHA256'])

    def test_unchanged_copied_executable_and_core_pass_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            args, expected = self.fixture(directory)
            with mock.patch.multiple(bp.run_mesen, **expected), contextlib.redirect_stdout(io.StringIO()):
                result = bp.run_mesen.run(args)
            receipt = json.loads((args.output / 'receipt.json').read_text())
            self.assertEqual(result, 0)
            self.assertTrue(receipt['runtime_identity_passed'])


if __name__ == '__main__':
    unittest.main()
