import re, unittest
from pathlib import Path
NSI = Path(__file__).resolve().parents[1] / "tools/windows_installer/setup.nsi"

class T(unittest.TestCase):
    def setUp(self): self.t = NSI.read_text()
    def install_section(self):
        return self.t.split('Section "Install"')[1].split("SectionEnd")[0]
    def test_legacy_paths_removed(self):
        s = self.install_section()
        for p in [r"$LOCALAPPDATA\ROTK2Windows\python", r"$LOCALAPPDATA\ROTK2Windows\app",
                  r"$DESKTOP\ROTK2.lnk", r"$SMPROGRAMS\ROTK2", r"Uninstall\ROTK2Windows",
                  r"$LOCALAPPDATA\KOEI_Original_Third_Branch\python_runtime",
                  r"$LOCALAPPDATA\Programs\KOEI_Classic_Enhanced"]:
            self.assertIn(p, s)
    def test_own_previous_install_removed(self):
        s = self.install_section()
        self.assertIn(r'RMDir /r "$INSTDIR\app"', s)
        self.assertIn(r'RMDir /r "$INSTDIR\python"', s)
    def test_saves_never_deleted(self):
        for line in self.t.splitlines():
            if re.match(r"\s*(RMDir|Delete)", line):
                self.assertNotIn("saves", line.lower())
                self.assertNotIn("KOEI_Original_Third_Branch\"", line)
    def test_removal_precedes_copy(self):
        s = self.install_section()
        self.assertLess(s.index("ROTK2Windows"), s.index('File /r "stage\\python"'))
if __name__ == "__main__": unittest.main()
