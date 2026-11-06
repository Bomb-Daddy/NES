import copy,json,unittest
from gk1_join_trace_report import validate_trace,TraceFormatError
U={'soldiers':40,'x':7,'y':7,'type':0,'mobility':10}
def ev(kind='unit_record_write',addr=0x6060,frame=100,cycle=900):
 return {'kind':kind,'addr':addr,'frame':frame,'cycle':cycle,'cpu_pc':0xF1C8,
 'vm_source_pointer':0xB110,'vm_source_prg_offset':0x15110,
 'unit0':dict(U),'unit1':dict(U),'scratch_word':1,'scratch_614c':14,'scratch_614e':5}
class Tests(unittest.TestCase):
 def lines(self,*e):return [json.dumps(v) for v in e]
 def test_positive_typed_events(self):
  r=validate_trace(self.lines(ev('native_handler_F1C8',0xF1C8),ev('word_613e_write',0x613e,100,901),ev('scratch_write',0x614e,101,902)))
  self.assertEqual((r['events'],len(r['word_writer_events']),len(r['native_handler_events'])),(3,1,1))
 def test_malformed(self):
  with self.assertRaises(TraceFormatError):validate_trace(['bad json'])
 def test_empty(self):
  with self.assertRaises(TraceFormatError):validate_trace([])
 def test_no_guess_of_cpu_pc(self):
  a=ev();a.pop('cpu_pc')
  with self.assertRaisesRegex(TraceFormatError,'missing'):validate_trace(self.lines(a))
 def test_counter_decrement(self):
  with self.assertRaisesRegex(TraceFormatError,'time order'):validate_trace(self.lines(ev(cycle=901),ev(cycle=900)))
 def test_outside_word_address(self):
  with self.assertRaisesRegex(TraceFormatError,'word address'):validate_trace(self.lines(ev('word_613e_write',0x613d)))
 def test_16bit_source_pointer(self):
  a=ev();a['vm_source_pointer']=0x10000
  with self.assertRaisesRegex(TraceFormatError,'vm_source_pointer'):validate_trace(self.lines(a))
 def test_boolean_not_int(self):
  a=ev();a['cycle']=True
  with self.assertRaisesRegex(TraceFormatError,'cycle'):validate_trace(self.lines(a))
 def test_mobility_overflow(self):
  a=ev();a['unit1']['mobility']=300
  with self.assertRaisesRegex(TraceFormatError,'unit state'):validate_trace(self.lines(a))
 def test_does_not_claim_proof_without_writes(self):
  a=validate_trace(self.lines(ev('native_handler_F1CD',0xF1CD)))
  self.assertEqual(a['word_writer_events'],[])
if __name__=='__main__':unittest.main()
