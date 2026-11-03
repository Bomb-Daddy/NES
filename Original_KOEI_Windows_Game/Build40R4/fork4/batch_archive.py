"""Fork Four per-campaign batch research save: complete normalized monthly history.

The .f4save SQLite database has one current game save and independent full snapshots
for every completed month. Source .ogsave format remains unchanged for Fork Three.
"""
from __future__ import annotations
import hashlib, json, sqlite3, zlib
from pathlib import Path
from third_branch.integrated import save_bytes, load_bytes


def _canonical(x):
    return json.dumps(x, sort_keys=True, separators=(',', ':'), ensure_ascii=False).encode()


def _zip(x):
    return zlib.compress(_canonical(x), 6)


class CampaignArchive:
    def __init__(self, path):
        self.path = Path(path)
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.db = sqlite3.connect(str(self.path))
        self.db.execute('PRAGMA foreign_keys=ON')
        self.db.executescript('''
        CREATE TABLE IF NOT EXISTS turn_index (
          completed_month INTEGER PRIMARY KEY, engine_month INTEGER NOT NULL,
          state_sha256 TEXT NOT NULL, whole_state_z BLOB NOT NULL,
          event_count INTEGER NOT NULL, advisor_count INTEGER NOT NULL);
        CREATE TABLE IF NOT EXISTS province_history (
          completed_month INTEGER NOT NULL, province_id TEXT NOT NULL,
          faction_id TEXT, state_z BLOB NOT NULL,
          PRIMARY KEY (completed_month, province_id),
          FOREIGN KEY (completed_month) REFERENCES turn_index(completed_month));
        CREATE TABLE IF NOT EXISTS officer_history (
          completed_month INTEGER NOT NULL, officer_id TEXT NOT NULL,
          faction_id TEXT, status TEXT, state_z BLOB NOT NULL,
          PRIMARY KEY (completed_month, officer_id),
          FOREIGN KEY (completed_month) REFERENCES turn_index(completed_month));
        CREATE TABLE IF NOT EXISTS faction_history (
          completed_month INTEGER NOT NULL, faction_id TEXT NOT NULL,
          state_z BLOB NOT NULL,
          PRIMARY KEY (completed_month, faction_id),
          FOREIGN KEY (completed_month) REFERENCES turn_index(completed_month));
        CREATE TABLE IF NOT EXISTS turn_events (
          completed_month INTEGER NOT NULL, event_ordinal INTEGER NOT NULL,
          event_id TEXT, actor TEXT, kind TEXT, event_z BLOB NOT NULL,
          PRIMARY KEY (completed_month, event_ordinal),
          FOREIGN KEY (completed_month) REFERENCES turn_index(completed_month));
        CREATE TABLE IF NOT EXISTS advisor_decisions (
          completed_month INTEGER NOT NULL, ordinal INTEGER NOT NULL,
          faction_id TEXT NOT NULL, advisor_id TEXT, effective_intelligence INTEGER,
          weight_bps INTEGER NOT NULL, changed INTEGER NOT NULL,
          decision_z BLOB NOT NULL,
          PRIMARY KEY (completed_month, ordinal),
          FOREIGN KEY (completed_month) REFERENCES turn_index(completed_month));
        CREATE TABLE IF NOT EXISTS current_save (id INTEGER PRIMARY KEY CHECK(id=1), completed_month INTEGER NOT NULL,
          game_save BLOB NOT NULL, game_save_sha256 TEXT NOT NULL,
          FOREIGN KEY(completed_month) REFERENCES turn_index(completed_month));
        ''')

    def record(self, c, completed_month, decisions=()):
        state = c.state
        if state['month'] != completed_month + 1:
            raise ValueError('Game month mismatch: rejecting incomplete historical snapshot')
        full = _canonical(state)
        sha = hashlib.sha256(full).hexdigest()
        provinces = state['core']['provinces']
        all_officers = state.get('officer_catalog') or state['core']['officers']
        factions = state['factions']
        log = state['log']
        existing = self.db.execute('SELECT state_sha256 FROM turn_index WHERE completed_month=?',(completed_month,)).fetchone()
        if existing:
            if existing[0] != sha:raise ValueError('Conflicting turn record')
            return False
        current = self.db.execute('SELECT completed_month FROM current_save WHERE id=1').fetchone()
        if current and completed_month != current[0] + 1:
            raise ValueError('Turn gap or fork detected; create a separate archive')
        if not current and completed_month != 0:
            raise ValueError('Turn-zero baseline missing')
        events = (list(log) if completed_month == 0 else [x for x in log if x.get('month') == completed_month])
        save = save_bytes(c)
        with self.db:
            self.db.execute('INSERT INTO turn_index VALUES(?,?,?,?,?,?)',
                            (completed_month,state['month'],sha,zlib.compress(full,6),len(events),len(decisions)))
            for pid,p in sorted(provinces.items()):
                self.db.execute('INSERT INTO province_history VALUES(?,?,?,?)',
                   (completed_month,pid,state['owners'][pid],_zip({
                        'core':p, 'extras':state['extras'][pid],
                        'market_stocks':state['market_stocks'][pid],
                        'governor':state['governors'].get(pid),
                        'neighbors':state['neighbors'].get(pid,()),
                        'metal_deposit':state.get('metal_deposits',{}).get(pid),
                        'owner':state['owners'][pid]})))
            for oid,base in sorted(all_officers.items()):
                runtime = state['core']['officers'].get(oid)
                self.db.execute('INSERT INTO officer_history VALUES(?,?,?,?,?)',
                   (completed_month,oid,state['officer_faction'].get(oid),
                    state.get('officer_availability',{}).get(oid,'active' if runtime else 'unposted'),
                    _zip({'catalog':base, 'runtime':runtime, 'faction':state['officer_faction'].get(oid),
                          'loyalty':state['officer_loyalty'].get(oid),
                          'officer_status':state.get('officer_availability',{}).get(oid)})))
            for fid,f in sorted(factions.items()):
                self.db.execute('INSERT INTO faction_history VALUES(?,?,?)',
                                (completed_month,fid,_zip({'faction':f, 'ruler_slot':state.get('ruler_slots',{}).get(fid),
                                     'ai_profile':state.get('ai_profiles',{}).get(fid)})))
            for n,event in enumerate(events):
                self.db.execute('INSERT INTO turn_events VALUES(?,?,?,?,?,?)',
                    (completed_month,n,event.get('id'),event.get('actor'),event.get('kind'),_zip(event)))
            for n,d in enumerate(decisions):
                meta=d.get('fork4_advisor',{})
                a=meta.get('advisor') or {}
                self.db.execute('INSERT INTO advisor_decisions VALUES(?,?,?,?,?,?,?,?)',
                    (completed_month,n,d.get('faction',''),a.get('officer_id'),a.get('intelligence'),
                     meta.get('weight_bps',0),int(meta.get('changed',False)),_zip(d)))
            self.db.execute('INSERT OR REPLACE INTO current_save VALUES(1,?,?,?)',
                            (completed_month,save,hashlib.sha256(save).hexdigest()))
        return True

    def cold_load(self):
        row=self.db.execute('SELECT game_save,game_save_sha256 FROM current_save WHERE id=1').fetchone()
        if row is None:raise ValueError('No save')
        if hashlib.sha256(row[0]).hexdigest()!=row[1]:raise ValueError('Current game save corrupted')
        c=load_bytes(row[0])
        last=self.db.execute('SELECT completed_month,state_sha256 FROM turn_index ORDER BY completed_month DESC LIMIT 1').fetchone()
        if last is None or c.state['month'] != last[0]+1:
            raise ValueError('Save/archive boundary mismatch')
        if hashlib.sha256(_canonical(c.state)).hexdigest()!=last[1]:
            raise ValueError('Saved state does not match history')
        return c

    def verify_counts(self, completed_month):
        counts={t:self.db.execute(f'SELECT COUNT(*) FROM {t} WHERE completed_month=?',(completed_month,)).fetchone()[0]
                for t in ('province_history','officer_history','faction_history')}
        state=json.loads(zlib.decompress(self.db.execute('SELECT whole_state_z FROM turn_index WHERE completed_month=?',(completed_month,)).fetchone()[0]))
        expected={'province_history':len(state['core']['provinces']),
                  'officer_history':len(state.get('officer_catalog') or state['core']['officers']),
                  'faction_history':len(state['factions'])}
        if counts!=expected:raise ValueError(f'Archive row discrepancy: {counts} != {expected}')
        return counts

    def close(self):
        self.db.close()
    def __enter__(self):return self
    def __exit__(self,*_):self.close()
