from pathlib import Path
import sys,json,re,subprocess,hashlib,collections,importlib.util,struct
out=Path('scratchpad/astra-port-audit-20260919')
spec=importlib.util.spec_from_file_location('cmp','pc_port/tests/field_vm_retail_compare.py');cmp=importlib.util.module_from_spec(spec);spec.loader.exec_module(cmp)
jobs=json.loads((out/'compiled.json').read_text());retail={};authority={};rows=[];missing=[]
pins={Path(p).name:h for h,p in (l.split() for l in Path('config/checksum.sha').read_text().splitlines() if l.strip())}
for overlay in sorted(name.removesuffix('.bin') for name in pins):
 binary=Path('disc')/('SLUS_006.64' if overlay=='slus_006.64' else overlay+'.bin')
 data=binary.read_bytes();digest=hashlib.sha256(data).hexdigest();assert digest==pins[binary.name.lower()],overlay
 functions={};checked=0
 for p in sorted((Path('asm')/overlay).rglob('*.s')):
  if 'data' in p.parts: continue
  name=None;insns=[]
  def save():
   if name and insns:
    if name not in functions or 'matchings' in p.parts or 'nonmatchings' in p.parts:
     functions[name]=insns[:]
  for line in p.read_text().splitlines():
   label=cmp.GLABEL.match(line)
   if label:
    save();name=label[1];insns=[];continue
   m=cmp.RETAIL_LINE.match(line)
   if m and name:
    off=int(m[1],16);expected=bytes.fromhex(m[3]);assert data[off:off+4]==expected,(p,hex(off));checked+=1
    insns.append((int(m[2],16),int.from_bytes(expected,'little'),m[4],m[5]))
  save()
 retail[overlay]=functions;authority[overlay]={'sha256':digest,'listing_words_verified':checked,'functions':len(functions)}
def parse_object_all(objdump, obj):
    text = subprocess.run([objdump, '-Drz', '-j', '.text', obj],
                          check=True, capture_output=True, text=True).stdout
    funcs = {}
    order = []
    cur = None
    for line in text.splitlines():
        m = cmp.OBJ_FUNC.match(line)
        if m:
            cur = m.group(2)
            funcs[cur] = {'start': int(m.group(1), 16), 'insns': []}
            order.append(cur)
            continue
        if cur is None:
            continue
        m = cmp.OBJ_RELOC.match(line)
        if m:
            off = int(m.group(1), 16)
            for ins in reversed(funcs[cur]['insns']):
                if ins['off'] == off:
                    ins['relocs'].append((m.group(2), m.group(3)))
                    break
            continue
        m = cmp.OBJ_INSN.match(line)
        if m:
            funcs[cur]['insns'].append({
                'off': int(m.group(1), 16), 'word': int(m.group(2), 16),
                'mn': m.group(3), 'ops': m.group(4), 'relocs': []})
    return funcs, order


def real_functions(obj):
 raw=Path(obj).read_bytes(); shoff=struct.unpack_from('<I',raw,32)[0]; ents,nsec,strings=struct.unpack_from('<HHH',raw,46)
 sections=[struct.unpack_from('<10I',raw,shoff+i*ents) for i in range(nsec)]
 names=raw[sections[strings][4]:sections[strings][4]+sections[strings][5]]
 def string(buf,off): return buf[off:buf.index(b'\0',off)].decode()
 textidx=next(i for i,s in enumerate(sections) if string(names,s[0])=='.text')
 symsec=next(s for s in sections if s[1]==2); st=sections[symsec[6]]; strings=raw[st[4]:st[4]+st[5]]
 symbols=[]; objects=[]
 for off in range(symsec[4],symsec[4]+symsec[5],symsec[9]):
  name,value,size,info,other,idx=struct.unpack_from('<IIIBBH',raw,off)
  if idx==textidx and info&15==1: objects.append(value)
  if idx==textidx and info&15==2 and size: symbols.append((string(strings,name),value,size))
 parsed,_=parse_object_all('mips-linux-gnu-objdump',obj)
 insns={x['off']:x for b in parsed.values() for x in b['insns']}
 result={}
 for name,start,size in symbols:
  size=min([start+size]+[v for v in objects if start<v<start+size])-start
  instructions=[]
  for off in range(start,start+size,4):
   if off not in insns: raise RuntimeError((obj,name,hex(off),'instruction missing'))
   instructions.append(insns[off])
  result[name]={'start':start,'insns':instructions}
 return result
def raw_compare(b,r,overlay,built):
 addresses={}
 for path in [Path('config/symbol_addrs.txt'),Path('config/symbol_addrs.slus_006.64.txt'),Path('config/symbol_addrs.'+overlay+'.txt')]:
  if path.exists():
   addresses.update({n:int(a,16) for n,a in re.findall(r'^\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;',path.read_text(),re.M)})
 addresses.update({n:v[0][0] for n,v in retail[overlay].items()})
 words=[]
 try:
  for i,ins in enumerate(b['insns']):
   word=ins['word']
   if len(ins['relocs'])>1: raise ValueError('multiple relocations')
   if ins['relocs']:
    kind,expr=ins['relocs'][0];sym,add=cmp.split_reloc_sym(expr)
    if sym=='.text' and kind=='R_MIPS_26':
     target=((word&0x03ffffff)<<2)+add
     owners=[(n,v) for n,v in built.items() if v['start']<=target<v['start']+4*len(v['insns']) and n in retail[overlay]]
     if len(owners)!=1: raise ValueError('ambiguous section-local jump')
     owner,v=owners[0];addr=retail[overlay][owner][0][0]+target-v['start']
     word=(word&0xfc000000)|((addr>>2)&0x03ffffff)
    else:
     addr=addresses.get(sym)
     if addr is None:
      match=re.fullmatch(r'(?:(?:func|D|jtbl|jpt)_|\.L)([0-9A-Fa-f]{8})',sym)
      if match: addr=int(match[1],16)
     if addr is None: raise ValueError('unknown symbol '+sym)
     if kind=='R_MIPS_26': word=(word&0xfc000000)|(((addr+((word&0x03ffffff)<<2))>>2)&0x03ffffff)
     elif kind=='R_MIPS_PC16': word=(word&0xffff0000)|(((addr+(cmp.sext16(word)<<2)-r[i][0])>>2)&0xffff)
     elif kind=='R_MIPS_LO16': word=(word&0xffff0000)|((addr+cmp.sext16(word))&0xffff)
     elif kind=='R_MIPS_GPREL16': word=(word&0xffff0000)|((addr+cmp.sext16(word)-0x80059170)&0xffff)
     elif kind=='R_MIPS_HI16':
      low=next((x for x in b['insns'][i+1:] if x['relocs'] and x['relocs'][0][0]=='R_MIPS_LO16' and cmp.split_reloc_sym(x['relocs'][0][1])[0]==sym),None)
      if low is None: raise ValueError('unpaired HI16 '+sym)
      value=addr+((word&0xffff)<<16)+cmp.sext16(low['word'])
      word=(word&0xffff0000)|(((value+0x8000)>>16)&0xffff)
     else: raise ValueError('unsupported '+kind)
   words.append(word)
  compiled=b''.join(struct.pack('<I',w) for w in words);original=b''.join(struct.pack('<I',x[1]) for x in r)
  return {'status':'BYTE_IDENTICAL' if compiled==original else 'BYTE_DIFFERENT','compiled_sha256':hashlib.sha256(compiled).hexdigest(),'retail_sha256':hashlib.sha256(original).hexdigest()}
 except ValueError as e: return {'status':'UNRESOLVED_RELOCATION','reason':str(e)}
for j in jobs:
 src=Path(j['source']);overlay=src.parts[1];native=Path(j['native_object']).exists()
 if overlay=='battle' and Path('pc_port/build_native/obj/battle_host_'+src.name+'.o').exists(): native=True
 if j['returncode']: missing.append(dict(source=str(src),reason='MIPS_COMPILE_FAILED'));continue
 built=real_functions(j['object'])
 preprocessed=Path(j['object'][:-1]+'i').read_text();included=set(re.findall(r'"/"\s+"(\w+)"\s+"\.s',preprocessed))
 for name,b in built.items():
  r=retail[overlay].get(name)
  row={'source':str(src),'overlay':overlay,'function':name,'native_source_compiled':native,'assembly_fallback':name in included,'mips_bytes':4*len(b['insns'])}
  if not r: row.update(status='NO_RETAIL_LISTING');rows.append(row);continue
  diffs=[]
  for i in range(min(len(b['insns']),len(r))):
   why=cmp.compare_insn(b['insns'][i],r[i],b['start'],r[0][0])
   if why: diffs.append({'instruction':i,'address':hex(r[i][0]),'reason':why,'compiled_word':f"{b['insns'][i]['word']:08x}",'retail_word':f'{r[i][1]:08x}'})
  status='EXACT' if len(b['insns'])==len(r) and not diffs else 'MISMATCH'
  row['raw_bytes']=raw_compare(b,r,overlay,built) if len(b['insns'])==len(r) else {'status':'SIZE_DIFFERENT'}
  if name=='func_8009E10C' and overlay=='field':
   import copy
   assert row['raw_bytes']['status']=='BYTE_IDENTICAL'
   mutant=copy.deepcopy(b);mutant['insns'][0]['word']^=1
   assert raw_compare(mutant,r,overlay,built)['status']=='BYTE_DIFFERENT'
   mutant=copy.deepcopy(b)
   call=next(i for i in mutant['insns'] if i['relocs'] and i['relocs'][0][0]=='R_MIPS_26')
   call['relocs']=[('R_MIPS_26','func_8009E10C')]
   assert raw_compare(mutant,r,overlay,built)['status']=='BYTE_DIFFERENT'
   print('RAW AUDIT WORD AND RELOCATION MUTANTS DETECTED',flush=True)
  row.update(status=status,retail_address=hex(r[0][0]),retail_bytes=4*len(r),differing_instructions=len(diffs),first_difference=diffs[0] if diffs else None)
  rows.append(row)
 print(str(src),len(built),'native',native,flush=True)
missing_functions=[{'overlay':overlay,'function':name,'retail_address':hex(v[0][0]),'retail_bytes':len(v)*4} for overlay,functions in retail.items() for name,v in functions.items() if (overlay,name) not in {(r['overlay'],r['function']) for r in rows}]
report={'retail_functions_without_compiled_C_TU_symbol':missing_functions,'scope':'Fresh MIPS object instruction comparison to hash-pinned retail listings; relocations checked symbolically by the repository comparator. Not native runtime equivalence.','authority':authority,'translation_units':len(jobs),'compile_failures':missing,'rows':rows}
(out/'comparison.json').write_text(json.dumps(report,indent=2))
print('TOTAL',dict(collections.Counter(r['status'] for r in rows)))
print('NATIVE SOURCE',dict(collections.Counter(r['status'] for r in rows if r['native_source_compiled'])))
