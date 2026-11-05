# NES KOEI — ChatGPT emulator references

**Start here for headless Mesen:** [Mesen Headless — ChatGPT Operations & Recovery Reference v1.00](MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md).

This reference explains the complete tested stack for executing an authorized original NES ROM with Mesen-for-AI, including environment preparation, first-run wizard diagnosis, native private save-state restoration, controller input, real-frame capture, command QA, evidence classification and recovery from timeouts.

**Executable smoke test:** [`headless_mesen_campaign_test.py`](../research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py) with [13 offline unit tests](../research/Genghis_Khan_NES/tools/test_headless_mesen_campaign_test.py). The test is intentionally nonautonomous; a full campaign driver requires separate development/testing.

**Current Temujin policy script:** [Unified AI Operating Script v2.00](../research/Genghis_Khan_NES/campaign/TEMUJIN_UNIFIED_AI_OPERATING_SCRIPT_v2.00.md) (operator decision rules, **not** executable Python code or evidence of a 30-year completed campaign).

**Important:** GitHub contains public reference code and documentation only. User-authorized original ROMs, modified ROMs, private Mesen full-machine `.mss` saves and copyrighted screenshots remain in private authorized storage. Reading this README does **not** install a native ChatGPT skill or give a chat session access to a shell/cloud computer.
