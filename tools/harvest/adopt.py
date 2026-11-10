#!/usr/bin/env python3
"""Adopt ex-machina (MIT) function bodies over Blizz INCLUDE_ASM stubs, byte-compare per function.
Usage: adopt.py <module> [--max-insns N] [--limit N] [--tu path] [--apply]"""
import re,os,sys,json,csv,glob,subprocess,collections,argparse,hashlib
sys.path.insert(0,os.path.dirname(__file__))
ap=argparse.ArgumentParser()
ap.add_argument('module'); ap.add_argument('--max-insns',type=int,default=60); ap.add_argument('--min-insns',type=int,default=0)
ap.add_argument('--limit',type=int,default=0); ap.add_argument('--tu',default=''); ap.add_argument('--out',default='')
ap.add_argument('--root',default=os.path.expanduser('~/dev/xenogears-decomp-public-harvest'))
ap.add_argument('--work',default=os.path.expanduser('~/scratch/xeno-mit-harvest'))
ap.add_argument('--source',default='xem'); ap.add_argument('--only',default=''); ap.add_argument('--diag',action='store_true'); ap.add_argument('--cc',default=''); ap.add_argument('--altcc',action='store_true'); ap.add_argument('--dump',default='')
a=ap.parse_args()
ROOT=a.root; WORK=a.work; MOD=a.module
os.chdir(ROOT)
XEM_SHA='20011c4'
EA_MODE=(a.source=='ea')
idx=json.load(open(WORK+('/ea_decl.json' if EA_MODE else '/xem_decl.json')))
EA_SHA='e54f35a'
eamap={'battling':'battling','field':'field','slus_006.64':'main','menu':'menu','member_change_menu':'menu_party','shop_menu':'menu_shop','movie':'movie','world_map':'worldmap'}
xmap={'battle':'battle','battle_command_file1':'ovl3087','battling':'menu','field':'field','member_change_menu':'ovl2598','menu':'slot39','movie':'movie','shop_menu':'ovl2601','slus_006.64':'resident','world_map':'worldmap'}
# XEM symbols (name -> (addr, type))
def load_syms(path):
    d={}
    for l in open(path):
        m=re.match(r'(\w+)\s*=\s*0x([0-9A-Fa-f]{8});\s*(?://\s*(.*))?',l)
        if m: d[m.group(1)]=(int(m.group(2),16),m.group(3) or '')
    return d
X=WORK+'/ext/xem/src/decomp/targets/'
xsym={}
if EA_MODE:
    E=WORK+'/ext/ea/config/us/symbols/'
    xsym.update(load_syms(E+'main.txt')); xsym.update(load_syms(E+eamap[MOD]+'.txt'))
else:
  xsym.update(load_syms(X+'resident/symbol_addrs.txt'))
if not EA_MODE and xmap[MOD]!='resident': xsym.update(load_syms(X+'overlays/%s.symbols.txt'%xmap[MOD]))
# Blizz known symbol names for this module
def toks(path,S):
    try: t=open(path,errors='ignore').read()
    except: return
    S.update(re.findall(r'[A-Za-z_]\w*',t))
linkable=set()
for f in glob.glob('linker/*.txt')+glob.glob('linker/*.ld'):
    if MOD in f or 'slus_006.64' in f: toks(f,linkable)
for f in glob.glob('asm/%s/**/*.s'%MOD,recursive=True): toks(f,linkable)
addr2names=collections.defaultdict(list)
for f in glob.glob('config/symbol_addrs.*.txt'):
    if MOD in f or 'slus_006.64' in f:
        for l in open(f):
            m=re.match(r'(\w+)\s*=\s*0x([0-9A-Fa-f]{8});',l)
            if m: addr2names[int(m.group(2),16)].append(m.group(1))
for f in glob.glob('linker/undefined_*_auto.*.txt'):
    if MOD in f or 'slus_006.64' in f:
        for l in open(f):
            m=re.match(r'(\w+)\s*=\s*0x([0-9A-Fa-f]{8});',l)
            if m: addr2names[int(m.group(2),16)].append(m.group(1))
N2A=globals().get("N2A",{})
for _a,_ns in addr2names.items():
    for _n in _ns: N2A.setdefault(_n,_a)
def blizz_name(addr,is_func):
    c=list(addr2names.get(addr,[]))+[('func_%08X' if is_func else 'D_%08X')%addr]
    for n in c:
        if n in linkable: return n
    return None
# Blizz-known type/macro names
bknown=set(['s8','s16','s32','u8','u16','u32','s64','u64','u_char','u_short','u_long','VECTOR','SVECTOR','MATRIX','CVECTOR','RECT','DVECTOR','NULL','bool','size_t'])
for f in glob.glob('include/**/*.h',recursive=True):
    t=open(f,errors='ignore').read()
    bknown.update(re.findall(r'#\s*define\s+(\w+)',t))
    bknown.update(re.findall(r'typedef[^;{]*?(\w+)\s*(?:\[[^\]]*\])?;',t))
    bknown.update(re.findall(r'}\s*(\w+)\s*;',t))
    bknown.update(re.findall(r'\b(?:struct|union|enum)\s+(\w+)\s*\{',t))
PSYQ=set(['s8','s16','s32','u8','u16','u32','s64','u64','u_char','u_short','u_long','u_int','VECTOR','SVECTOR','MATRIX','CVECTOR','RECT','DVECTOR','NULL','bool','size_t'])
for f in glob.glob('include/psyq/**/*.h',recursive=True)+['include/types.h','include/common.h']:
    try: t=open(f,errors='ignore').read()
    except: continue
    PSYQ.update(re.findall(r'#\s*define\s+(\w+)',t)); PSYQ.update(re.findall(r'typedef[^;{]*?(\w+)\s*(?:\[[^\]]*\])?;',t))
    PSYQ.update(re.findall(r'}\s*(\w+)\s*;',t)); PSYQ.update(re.findall(r'\b(?:struct|union|enum)\s+(\w+)\s*\{',t))
KW=set('auto break case char const continue default do double else enum extern float for goto if int long register return short signed sizeof static struct switch typedef union unsigned void volatile while asm __asm__ inline'.split())
def strip_str(s): return re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'','""',s)
def ident_tokens(s):
    s=strip_str(s); out=[]
    for m in re.finditer(r'[A-Za-z_]\w*',s):
        st=m.start()
        pre=s[max(0,st-2):st]
        if pre.endswith('.') or pre.endswith('->'): continue
        if m.group(0) in KW: continue
        out.append(m.group(0))
    return out
VIS=set()
def visible_for(tu):
    obj='build/'+tu+'.o'
    cmds=run('ninja -t commands %s'%obj).stdout.split('\n')
    cpp=[c for c in cmds if 'mips-linux-gnu-cpp' in c and ('src/'+tu.split('src/',1)[-1]) in c][:1]
    v=set(['s8','s16','s32','u8','u16','u32','u_char','u_short','u_long','NULL'])
    if cpp:
        c=cpp[0]; c=re.sub(r'\s-o\s+\S+',' -o /tmp/xm_vis.dm',c); c=c.replace(' -P ',' -dM ').replace(' -MMD','').replace(' -MP','')
        r=run(c)
        try: t=open('/tmp/xm_vis.dm').read()
        except: t=''
        v.update(re.findall(r'#define\s+(\w+)',t))
    # typedef names from the base preprocessed file
    try: t=open(WORK+'/base_build/'+tu+'.i').read()
    except: t=''
    if not t:
        try: t=open('build/'+tu+'.i').read()
        except: t=''
    v.update(re.findall(r'typedef[^;{]*?(\w+)\s*(?:\[[^\]]*\])*;',t))
    v.update(re.findall(r'}\s*(\w+)\s*;',t))
    v.update(re.findall(r'\b(?:struct|union|enum)\s+(\w+)',t))
    v.update(re.findall(r'\(\s*\*\s*(\w+)\s*\)\s*\(',t))
    return v
def pick(entries,kinds,pref_file=None):
    c=[e for e in entries if e['kind'] in kinds]
    if pref_file:
        c2=[e for e in c if e['file']==pref_file]
        if c2: c=c2
    # prefer header extern for vars
    c.sort(key=lambda e:(not e['src'].lstrip().startswith('extern'), e['file'].endswith('.c')))
    return c[0] if c else None
def split_top_commas(s):
    out=[];d=0;cur=''
    for ch in s:
        if ch in '([': d+=1
        if ch in ')]': d-=1
        if ch==',' and d==0: out.append(cur); cur=''
        else: cur+=ch
    out.append(cur); return out
_ro_cache={}
def has_ro(nm):
    if nm not in _ro_cache:
        fs=glob.glob('asm/%s/nonmatchings/**/%s.s'%(MOD,nm),recursive=True)
        _ro_cache[nm]=bool(fs) and 'section .rodata' in open(fs[0],errors='ignore').read()
    return _ro_cache[nm]
CCMAP={'2.6.3':'gcc-2.6.3-psx','2.7.2':'gcc-2.7.2-psx','2.7.2-cdk':'gcc-2.7.2-cdk-psx','2.6.0':'gcc-2.6.0-psx'}
cur_xcc=['']
class Reject(Exception): pass
def analyze(row,fe,mode=''):
    """Return (block_text) or raise Reject."""
    name=row['xem_name']; src=fe['src']
    if re.search(r'\basm\b|__asm__',strip_str(src)): raise Reject('switch/float/asm')
    if re.search(r'\bswitch\b|\bfloat\b|\bdouble\b|\b\d+\.\d',strip_str(src)) or '"' in src:
        row['ro']=True
        if not has_ro(row['blizz_name']): raise Reject('rodata not migrated into function .s')
    pf=fe['file']; decls=[]; typeents=[]; seen_t=set(); symdone={}
    base_toks=ident_tokens(src)
    renames={}
    ISO='iso' in mode; ASMDEF='asmdef' in mode; tmap={}
    def isotype(n): return ISO and n not in PSYQ and any(e['kind'] in ('typedef','tag') for e in (idx.get(n,[])+idx.get('tag:'+n,[])))
    def add_type(n):
        if n in seen_t or (n in VIS and not isotype(n)): return
        if isotype(n): tmap[n]='xm_'+n
        seen_t.add(n)
        es=idx.get(n) or idx.get('tag:'+n)
        if not es: return
        e=pick(es,('typedef','define','tag'))
        if not e: return
        for t2 in ident_tokens(e['src']):
            if t2!=n: resolve(t2,e['src'],in_type=(e['kind']!='define'))
        key=hashlib.md5(e['src'].encode()).hexdigest()
        if key not in [k for k,_ in typeents]: typeents.append((key,(n,e['src'])))
    def decl_for(n):
        addr,ty=xsym[n]; is_func=('func' in ty) 
        bn=blizz_name(addr,is_func)
        if not bn: raise Reject('no linkable blizz symbol for %s @%08X'%(n,addr))
        es=idx.get(n,[])
        if is_func:
            e=pick(es,('proto','funcdef'),pf)
            if not e: raise Reject('no proto for '+n)
            head=e['src'].split('{')[0].strip().rstrip(';').strip()
            head=re.sub(r'^static\s+','',head)
            head=re.sub(r'^extern\s+','',head)
            for t2 in ident_tokens(head):
                if t2!=n: resolve(t2,head,in_type=True)
            d=re.sub(r'\b%s\b'%re.escape(n),'xm_'+n,head,count=1)
            return '%s __asm__("%s");'%(d,bn)
        e=pick(es,('var',),pf)
        if not e: raise Reject('no var decl for '+n)
        s=e['src'].strip().rstrip(';')
        s=re.sub(r'\s*=.*','',s,flags=re.S)
        s=re.sub(r'^static\s+','',s)
        if not s.startswith('extern'): s='extern '+s
        parts=split_top_commas(s)
        m=re.match(r'(.*?)(\*?\s*\w+\s*(?:\[[^\]]*\])*)\s*$',parts[0],re.S)
        if not m: raise Reject('var parse '+n)
        base=m.group(1); dl=[m.group(2)]+parts[1:]
        mine=[x for x in dl if re.search(r'\b%s\b'%re.escape(n),x)]
        if not mine: raise Reject('var declarator '+n)
        s=base.rstrip()+' '+mine[0].strip()
        for t2 in ident_tokens(s):
            if t2!=n: resolve(t2,s,in_type=True)
        s=re.sub(r'\b%s\b'%re.escape(n),'xm_'+n,s,count=1)
        return '%s __asm__("%s");'%(s,bn)
    def resolve(t,ctx,in_type=False):
        if t in symdone or (t in VIS and not isotype(t)): return
        if t in xsym and not in_type:
            symdone[t]=decl_for(t); renames[t]='xm_'+t; return
        if t in xsym and in_type:
            return
        es=idx.get(t) or idx.get('tag:'+t)
        if es:
            if any(e['kind'] in ('typedef','define','tag') for e in es): add_type(t); return
            if not in_type and any(e['file']==pf and e['kind'] in('var','funcdef','proto') for e in es):
                raise Reject('file-local symbol '+t)
    for t in dict.fromkeys(base_toks):
        if t==name and t in xsym: 
            # self reference (recursion) -> symbol decl
            symdone[t]=decl_for(t); renames[t]='xm_'+t; continue
        resolve(t,src)
    # rename function def name and symbols
    body=src.strip()
    head,rest=body.split('{',1)
    head=re.sub(r'^static\s+','',head)
    head=re.sub(r'\b%s\b(?=\s*\()'%re.escape(name),row['blizz_name'],head,count=1)
    body=head+'{'+rest
    for n,r in renames.items():
        if n==name: pass
        body=re.sub(r'(?<![\w.>])%s\b'%re.escape(n),r,body)
    # undo accidental rename of the defined name in head
    body=body.replace('xm_'+name+'(',row['blizz_name']+'(',1) if renames.get(name) and body.find('xm_'+name+'(')>=0 and body.find('xm_'+name+'(')<body.find('{') else body
    out=['#ifndef XENO_PC_PORT',
         ('/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @%s): %s (%s). */'%(EA_SHA,name,row['xem_loc']) if EA_MODE else '/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @%s): %s (%s). */'%(XEM_SHA,name,row['xem_loc']))]
    def tren(txt):
        for n0,r0 in tmap.items(): txt=re.sub(r'(?<![\w.>])%s\b'%re.escape(n0),r0,txt)
        return txt
    for key,(n,s) in typeents:
        if s.lstrip().startswith('#'):
            for n0,r0 in renames.items(): s=re.sub(r'(?<![\w.>])%s\b'%re.escape(n0),r0,s)
        else: s=tren(s)
        out.append('#ifndef XM_T_%s\n#define XM_T_%s\n%s\n#endif'%(tmap.get(n,n),tmap.get(n,n),s))
    for n,d in symdone.items():
        out.append('#ifndef XM_S_%s\n#define XM_S_%s\n%s\n#endif'%(n,n,tren(d)))
    body=tren(body)
    if ASMDEF:
        hd=body.split('{',1)[0].rstrip()
        hd=hd.replace(row['blizz_name']+'(','xm_'+name+'(',1)
        body=hd+' {'+body.split('{',1)[1]
        out.append('%s __asm__("%s");'%(hd,row['blizz_name']))
    out.append(body)
    out.append('#endif')
    return '\n'.join(out)+'\n'
def run(cmd,**k):
    return subprocess.run(cmd,shell=True,capture_output=True,text=True,**k)
N2A=globals().get("N2A",{})
def symaddr(n):
    m=re.match(r'^(?:D|func)_([0-9A-Fa-f]{8})$',n)
    if m: return int(m.group(1),16)
    return N2A.get(n)
def sx16(v): return v-0x10000 if v&0x8000 else v
def local_syms(obj):
    d={}
    for l in run('mips-linux-gnu-objdump -t %s'%obj).stdout.split('\n'):
        m=re.match(r'^([0-9a-f]{8}) (.{7}) (\.rodata|\.data|\.sdata|\.bss|\.sbss)\s+[0-9a-f]{8}\s+(\S+)',l)
        if m: d[m.group(4)]=(m.group(3),int(m.group(1),16))
    return d
def func_dump(obj):
    LS=local_syms(obj)
    r=run('mips-linux-gnu-objdump -d -r -j .text %s'%obj)
    items=[]; cur=None
    for l in r.stdout.split('\n'):
        m=re.match(r'^[0-9a-f]{8} <([^>]+)>:',l)
        if m:
            if not m.group(1).startswith('.L'): cur=m.group(1)
            continue
        if cur is None: continue
        m=re.match(r'^\s+[0-9a-f]+:\s+([0-9a-f]{8})\s',l)
        if m: items.append([cur,int(m.group(1),16),None]); continue
        m=re.match(r'^\s+[0-9a-f]+: (R_MIPS_\w+)\s+(\S+)',l)
        if m and items: items[-1][2]=(m.group(1),m.group(2))
    funcs=collections.OrderedDict()
    for i,(fn,w,rl) in enumerate(items):
        funcs.setdefault(fn,[])
        out=funcs[fn]
        if rl is None: out.append('%08x'%w); continue
        t,sy=rl; nm0=sy.split('+')[0]; sec=None; off=0
        if nm0 in LS: sec,ad=LS[nm0]
        else: ad=symaddr(nm0)
        if '+' in sy:
            try: off=int(sy.split('+')[1],16)
            except: ad=None
        if ad is None: out.append('%08x %s %s'%(w,t,sy)); continue
        ad+=off
        pf='' if sec is None else sec+':'
        if t=='R_MIPS_26': out.append('J %s%08x'%(pf,ad+((w&0x3ffffff)<<2)))
        elif t=='R_MIPS_LO16': out.append('%04x LO %s%08x'%(w>>16,pf,ad+sx16(w&0xffff)))
        elif t=='R_MIPS_HI16':
            lo=0
            for (_,w2,r2) in items[i+1:i+40]:
                if r2 and r2[0]=='R_MIPS_LO16' and r2[1]==sy: lo=sx16(w2&0xffff); break
            out.append('%04x HI %s%08x'%(w>>16,pf,ad+((w&0xffff)<<16)+lo))
        else: out.append('%08x %s %s'%(w,t,sy))
    return funcs
def flat(d): return [x for v in d.values() for x in v]
def disasm_func(obj,name):
    r=run('mips-linux-gnu-objdump -d -r -j .text %s'%obj); out=[]; on=False
    for l in r.stdout.split('\n'):
        m=re.match(r'^[0-9a-f]{8} <([^>]+)>:',l)
        if m:
            if not m.group(1).startswith('.L'): on=(m.group(1)==name)
            continue
        if on and l.strip(): out.append(re.sub(r'^\s+[0-9a-f]+:\s+','',l).replace('\t',' '))
    return out
def sec_dump(obj):
    r=run('mips-linux-gnu-objdump -s -r -j .rodata -j .data -j .sdata -j .bss -j .sbss %s'%obj)
    return re.sub(r'^.*file format.*$','',r.stdout,flags=re.M)
def main():
    rows=[d for d in csv.DictReader(open(WORK+'/HARVEST.csv')) if d['module']==MOD and d['blizz_status']=='INCLUDE_ASM' and a.min_insns<=int(d['insns'])<=a.max_insns and ((d['ea_conf']=='aligned-by-order' and d['ea_func']) if EA_MODE else d['xem_status']=='C')]
    if EA_MODE:
        for d in rows: d['xem_name']=d['ea_func']; d['xem_loc']=d['ea_file']
        excl=set(l.split()[0] for l in open(WORK+'/ea_skip.txt')) if os.path.exists(WORK+'/ea_skip.txt') else set()
        rows=[d for d in rows if d['blizz_name'] not in excl]
    rows.sort(key=lambda d:int(d['insns']))
    # find compiled TU (object) per row via the base preprocessed files, and the live INCLUDE_ASM source line
    ihash={}
    for f in glob.glob(WORK+'/base_build/src/%s/**/*.c.i'%MOD,recursive=True):
        t=open(f,errors='ignore').read()
        for m in re.finditer(r'__maspsx_include_asm_hack_(\w+)\(\)',t): ihash.setdefault(m.group(1),[]).append(f)
    srcs=[f for f in glob.glob('src/%s/**/*.c'%MOD,recursive=True)+glob.glob('src/%s/**/*.inc'%MOD,recursive=True)]
    stext={f:open(f,errors='ignore').read() for f in srcs}
    bytu=collections.defaultdict(list)
    res=[]
    for r in rows:
        nm=r['blizz_name']
        if a.only and nm not in a.only.split(','): continue
        ifs=ihash.get(nm,[])
        if len(ifs)!=1: res.append((nm,r['addr'],'reject','compiled in %d TUs'%len(ifs))); continue
        tuobj=ifs[0].split('/base_build/')[1][:-2]   # src/.../x.c  (from x.c.i)
        pat=re.compile(r'^[ \t]*INCLUDE_ASM\(\s*[^,]+,\s*%s\s*\)\s*;[ \t]*$'%re.escape(nm),re.M)
        cand=[f for f,t in stext.items() if pat.search(t) and 'Never compiled' not in t[:1500]]
        if len(cand)!=1: res.append((nm,r['addr'],'reject','%d live INCLUDE_ASM files'%len(cand))); continue
        if a.tu and tuobj!=a.tu: continue
        r=dict(r); r['edit']=cand[0]; bytu[tuobj].append(r)
    n=0
    def jobs():
        for tu,rs in bytu.items():
            if not a.altcc: yield tu,rs; continue
            tg=re.search(r'gcc-2\.\d\.\d(?:-cdk)?-psx',run('ninja -t commands build/%s.o'%tu).stdout)
            tg=tg.group(0) if tg else ''
            for xc in sorted(set(r['xem_cc'] for r in rs)):
                if CCMAP.get(xc) in (None,tg): continue
                yield tu,[r for r in rs if r['xem_cc']==xc]
    for tu,rs in jobs():
        cur_xcc[0]=rs[0]['xem_cc']
        edits=sorted(set(r['edit'] for r in rs)); origs={f:open(f).read() for f in edits}
        VIS.clear(); VIS.update(visible_for(tu))
        cands=[];
        for r in rs:
            if a.limit and n>=a.limit: break
            es=idx.get(r['xem_name'],[])
            fe=None
            for e in es:
                if e['kind']=='funcdef' and e['file']==r['xem_loc'].split(':')[0]: fe=e
            if not fe:
                res.append((r['blizz_name'],r['addr'],'reject','no funcdef')); continue
            try: blk=analyze(r,fe)
            except Reject as ex:
                res.append((r['blizz_name'],r['addr'],'reject',str(ex))); continue
            r['mk']=(lambda r=r,fe=fe: (lambda md: analyze(r,fe,md)))()
            if a.dump and r['blizz_name']==a.dump: print(blk)
            cands.append((r,blk)); n+=1
        if not cands: continue
        obj='build/'+tu+'.o'
        def restore():
            for f,t in origs.items(): open(f,'w').write(t)
        def attempt(cs):
            ts=dict(origs)
            for r,blk in cs:
                pat=re.compile(r'^[ \t]*INCLUDE_ASM\(\s*[^,]+,\s*%s\s*\)\s*;[ \t]*$'%re.escape(r['blizz_name']),re.M)
                ts[r['edit']],k=pat.subn(lambda m:blk.rstrip('\n'),ts[r['edit']],count=1)
            for f,t in ts.items(): open(f,'w').write(t)
            if a.altcc:
                cmds=run('ninja -t commands %s'%obj).stdout.strip().split('\n')
                gc=CCMAP.get(cur_xcc[0])
                p=None
                for c in cmds:
                    c=re.sub(r'gcc-2\.\d\.\d(?:-cdk)?-psx',gc,c)
                    p=run('nice -n 10 '+c+' 2>&1')
                    if p.returncode!=0: break
                return p
            p=run('nice -n 10 ninja -j4 %s 2>&1'%obj)
            return p
        cs=list(cands)
        def good_set(c):
            if not c: return []
            p=attempt(c)
            if p.returncode==0: return list(c)
            if len(c)==1:
                r0=c[0][0]
                if not r0.get('modes_tried'):
                    r0['modes_tried']=True; r0['ro']=r0.get('ro')
                    for md in ('asmdef','iso','iso+asmdef'):
                        try: nb=r0['mk'](md)
                        except Reject: continue
                        r0['mode']=md
                        if attempt([(r0,nb)]).returncode==0: return [(r0,nb)]
                    r0['mode']=''
                er=(re.findall(r'build/[^\n]*\.i:\d+:[^\n]*',p.stdout) or re.findall(r'[^\n]*(?:error|Error)[^\n]*',p.stdout) or [p.stdout.strip()[-120:]])[0][:200]
                if a.diag: print('CERR',c[0][0]['blizz_name'],p.stdout[:1500])
                res.append((c[0][0]['blizz_name'],c[0][0]['addr'],'compile-error',er)); return []
            mid=len(c)//2
            return good_set(c[:mid])+good_set(c[mid:])
        cs=good_set(cs)
        if cs:
            p=attempt(cs)
            if p.returncode!=0:
                res.append(('*TU*',tu,'abort','combined compile fails')); cs=[]
        for rnd in range(100):
            if not cs: break
            p=attempt(cs)
            if p.returncode!=0: res.append(('*TU*',tu,'abort','recompile fails')); cs=[]; break
            base=func_dump(WORK+'/base_build/'+tu+'.o'); new=func_dump(obj)
            bad=[]
            itxt=open('build/'+tu+'.i',errors='ignore').read()
            for r,blk in cs:
                nm=r['blizz_name']
                if not re.search(r'\b(?:%s|xm_%s)\s*\([^;{]*\)\s*\{'%(re.escape(nm),re.escape(r['xem_name'])),itxt):
                    res.append((nm,r['addr'],'reject','definition not in compiled TU (dead region)')); bad.append((r,blk,1)); continue
                if base.get(nm) and nm in new and base.get(nm)==new.get(nm): continue
                bad.append((r,blk))
            if not bad: break
            order={k:i for i,k in enumerate(base)}
            bad.sort(key=lambda b:order.get(b[0]['blizz_name'],1<<30))
            sz=[i for i,b in enumerate(bad) if len(b)==2 and len(base.get(b[0]['blizz_name'],[]))!=len(new.get(b[0]['blizz_name'],[]))]
            if sz: bad=bad[:sz[0]+1]   # later diffs may only be shifted by this one; re-check next round
            swapped=False
            for b in list(bad):
                r,blk=b[0],b[1]
                if len(b)==2 and not r.get('mm_tried'):
                    r['mm_tried']=True
                    for md in ('iso',):
                        try: nb=r['mk'](md)
                        except Reject: continue
                        if nb==blk: continue
                        if attempt([(r,nb)]).returncode==0:
                            cs[cs.index((r,blk))]=(r,nb); bad.remove(b); swapped=True; break
            if swapped and not bad: continue
            for b in bad:
                r,blk=b[0],b[1]
                if len(b)==2 and a.diag:
                    import difflib
                    nm=r['blizz_name']
                    print('=== DIAG',nm); print('\n'.join(list(difflib.unified_diff(disasm_func(WORK+'/base_build/'+tu+'.o',nm),disasm_func(obj,nm),'base','new',lineterm='',n=1))[:60]))
                if len(b)==2:
                    nb=len(base.get(r['blizz_name'],[])); nn=len(new.get(r['blizz_name'],[]))
                    res.append((r['blizz_name'],r['addr'],'mismatch','base %d new %d words/relocs'%(nb,nn)))
                cs.remove((r,blk))
        if a.altcc:
            for r,blk in cs: res.append((r['blizz_name'],r['addr'],'ALT-MATCH',CCMAP[cur_xcc[0]]))
            restore(); run('nice -n 10 ninja -j4 %s'%obj); continue
        if not cs: restore()
        # final: compare full-object sections
        bo=WORK+'/base_build/'+tu+'.o'
        def final_ok(c):
            p=attempt(c)
            if p.returncode!=0: return False
            ok=sec_dump(obj)==sec_dump(bo) and flat(func_dump(obj))==flat(func_dump(bo))
            if not ok and a.diag:
                import difflib
                print('SECDIFF',len(c)); print('\n'.join(list(difflib.unified_diff(sec_dump(bo).split('\n'),sec_dump(obj).split('\n'),lineterm='',n=0))[:20]))
            return ok
        if cs and not final_ok(cs):
            ro=[x for x in cs if x[0].get('ro')]; plain=[x for x in cs if not x[0].get('ro')]
            if ro and plain and final_ok(plain):
                cs=list(plain)
                for x in ro:
                    if final_ok(cs+[x]): cs.append(x)
                    else: res.append((x[0]['blizz_name'],x[0]['addr'],'ro-mismatch','rodata/section differs with this function'))
            elif ro and not plain:
                keep=[]
                for x in ro:
                    if final_ok(keep+[x]): keep.append(x)
                    else: res.append((x[0]['blizz_name'],x[0]['addr'],'ro-mismatch','rodata/section differs with this function'))
                cs=keep
            else:
                res.append(('*TU*',tu,'section-diff','final section compare failed; reverted TU')); cs=[]
            if cs: final_ok(cs)
            else:
                restore(); run('nice -n 10 ninja -j4 %s'%obj)
        for r,blk in cs: res.append((r['blizz_name'],r['addr'],'ADOPTED',r['xem_name']))
    out=a.out or WORK+'/adopt_%s.json'%MOD
    json.dump(res,open(out,'w'),indent=0)
    c=collections.Counter(x[2] for x in res); print(dict(c))
    rj=collections.Counter(x[3].split(' @')[0][:30] for x in res if x[2]=='reject'); print(rj.most_common(8))
main()
