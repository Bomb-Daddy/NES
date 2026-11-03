import pickle
import os
ROM=open(os.environ.get('GK_ROM','Genghis Khan (USA).nes'),'rb').read()
def rb(bank,a):
    if a>=0xC000: return ROM[16+15*0x4000+(a-0xC000)]
    return ROM[16+bank*0x4000+(a-0x8000)]
def oplen(op):
    if 0x80<=op<=0x9F: return 1 if op&1 else 2
    if 0xA0<=op<=0xAF: return 2
    if op in (0xB7,): return 1
    if op in (0xD6,0xD7,0xD8,0xDE,0xDF,0xAC): return 2
    if op in (0xE9,): return 3
    if op in (0xE3,0xE4,0xE5,0xE6,0xE7): return 1
    return 0
def name(op):
    if op<=0x0B: return 'ACC=L%d'%op
    if op<=0x0F: return 'ACC=ARG%d'%(op-0x0C)
    if op<=0x1B: return 'B=L%d'%(op-0x10)
    if op<=0x1F: return 'B=ARG%d'%(op-0x1C)
    if op<=0x2B: return 'L%d=ACC'%(op-0x20)
    if op<=0x2F: return 'ARG%d=ACC'%(op-0x2C)
    if op<=0x3F: return 'PUSH L/ARG %X'%(op-0x30)
    if op==0x40: return 'ACC=0'
    if op<=0x4F: return 'ACC=%d'%(op&15)
    if op==0x50: return 'B=0'
    if op<=0x5F: return 'B=%d'%(op&15)
    if op<=0x6F: return 'PUSH %d'%(op&15)
    if op==0x70: return 'NOP'
    if op<=0x7F: return 'ACC+=%d'%(op&15)
    d={0xB0:'ACC=W[ACC]',0xB1:'W[pop]=ACC',0xB3:'PUSH ACC',0xB4:'B=pop',0xB5:'ACC*=B',0xB8:'ACC/=B',0xB9:'ACC%=B',0xBA:'ACC%=B',0xBB:'ACC+=B',0xBC:'ACC-=B',0xBD:'ACC<<=B',0xBE:'ACC>>=B?',
       0xC0:'EQ',0xC1:'NE',0xC2:'LT',0xC3:'LE',0xC4:'GT',0xC5:'GE',0xC6:'ULT',0xC7:'ULE',0xC8:'UGT',0xC9:'UGE',0xCA:'ACC=!ACC',0xCB:'NEG',0xCC:'NOT',0xCD:'SWAP',0xCF:'RET',
       0xD0:'INC',0xD1:'DEC',0xD2:'SHL1',0xD3:'ACC=B[ACC]',0xD4:'B[pop]=ACC',0xD6:'JMP',0xD7:'JNZ',0xD8:'JZ',0xDA:'AND',0xDB:'OR',0xDC:'XOR',0xAC:'CALL',0xE9:'CALL3',0xB7:'SYS',
       0x8F:'ACC+=imm8',0x89:'ACC=imm8',0x8A:'ACC=imm16',0x90:'ACC+=imm16',0x8C:'B=imm16',0x8D:'PUSH imm8',0x8E:'PUSH imm16',0x8B:'B=imm8',0xA6:'B=W[abs]',0xAA:'PUSH W[abs]',0xA4:'ACC=W[abs]',0xA8:'W[abs]=ACC',0xDE:'ACC=fp+',0xDF:'B=fp+'}
    if op in d: return d[op]
    if 0xA0<=op<=0xAF: return 'ABS[%X]'%op
    if 0x80<=op<=0x9F: return 'M%02X'%op
    return '?%02X'%op
def fmt(bank,vpc,op):
    a=vpc; n=oplen(op); ops=[rb(bank,a+i) for i in range(n)]
    if n==2: o='$%04X'%(ops[0]|ops[1]<<8)
    elif n==1: o='$%02X'%ops[0]
    elif n==3: o=' '.join('%02X'%x for x in ops)
    else: o=''
    return '%02X %s %s'%(op,name(op),o)
def dump(bank,a,b):
    while a<b:
        op=rb(bank,a); n=oplen(op); print('%04X: %s'%(a,fmt(bank,a+1,op))); a+=1+n
