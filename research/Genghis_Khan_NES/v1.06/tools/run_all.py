"""Run source replays, plus captured-evidence checks when GK_EVIDENCE is set."""
import importlib,os,sys,unittest
names=['replay_b7_arithmetic','replay_da67','replay_da8b','replay_dcea','verify_price_paths']
if os.environ.get('GK_EVIDENCE'):names.append('verify_live_evidence')
suite=unittest.TestSuite()
for name in names:suite.addTests(unittest.defaultTestLoader.loadTestsFromModule(importlib.import_module(name)))
r=unittest.TextTestRunner(verbosity=2).run(suite)
sys.exit(not r.wasSuccessful())
