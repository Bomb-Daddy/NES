exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore('fall1175');snap('ally_trace_before')
bp=call('breakpoint.create',memoryType='nesMemory',address=0x6e0d,length=2,access='write',cpuType='nes')
press('down',5,60);snap('ally_trace_refuse_selected');press('a',5,600)
regs=call('cpu.registers',cpuType='nes');state={'registers':regs,'vm_pc':word(6),'vm_a':word(8),'vm_frame':word(4),'vm_stack':word(2),'bank_header':list(memory(0x8000,16)),'frame_bytes':list(memory(word(4)-16,64)),'stack_bytes':list(memory(word(2)-16,64)),'gold1':word(0x6e0d),'gold8':word(0x6dcd+7*83+64),'breakpoints':call('breakpoint.list')}
(OUT/'ally_gold_writer.json').write_text(json.dumps(state,indent=2))
call('breakpoint.delete',handle=bp['handle']);step(600);snap('ally_trace_after');RESULTS=state
