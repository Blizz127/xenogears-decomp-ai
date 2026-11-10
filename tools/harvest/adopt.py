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
ap.add_argument('--source',default='xem'); ap.add_argument('--only',default=''); ap.add_argument('--diag',action='store_true'); ap.add_argument('--cc',default=''); ap.add_argument('--altcc',action='store_true'); ap.add_argument('--hand',default=''); ap.add_argument('--dump',default=''); ap.add_argument('--prelude',action='store_true'); ap.add_argument('--pos',action='store_true'); ap.add_argument('--nojt',action='store_true')
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
if not EA_MODE:
    for _f in sorted(glob.glob(X+'overlays/*.symbols.txt')):
        for _k,_v in load_syms(_f).items(): xsym.setdefault(_k,_v)
if not EA_MODE:
    try:
        for _k,_v in json.load(open(WORK+'/ext/xem/src/port/symbols.json')).items():
            if isinstance(_v,int) and _k not in xsym: xsym[_k]=(_v,'')
    except Exception as _e: pass
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
GTE_T={'VECTOR','SVECTOR','MATRIX','CVECTOR','DVECTOR'}
def psyq_pre(blk):
    if a.pos: return blk
    need=[t for t in set(ident_tokens(blk)) if t in GTE_T and t not in VIS]
    if need and 'psyq/libgte.h' not in blk:
        return '#ifndef XENO_PC_PORT\n#include "psyq/libgte.h"\n#endif\n'+blk
    return blk
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
MACROS=[set()]
CTXF=['']
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
        v.update(re.findall(r'#define\s+(\w+)',t)); MACROS[0]=set(re.findall(r'#define\s+(\w+)',t))
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
POSMAP={}
def build_posmap(tu):
    """names -> list of (file,line) of typedef/struct definitions seen by the TU's own cpp run (linemarkers kept)"""
    POSMAP.clear()
    obj='build/'+tu+'.o'
    cmds=run('ninja -t commands %s'%obj).stdout.split('\n')
    cpp=[c for c in cmds if 'mips-linux-gnu-cpp' in c and ('src/'+tu.split('src/',1)[-1]) in c][:1]
    if not cpp: return False
    c=re.sub(r'\s-o\s+\S+',' -o /tmp/xm_pos.i',cpp[0]).replace(' -P ',' ').replace(' -MMD','').replace(' -MP','')
    run(c)
    try: t=open('/tmp/xm_pos.i',errors='ignore').read()
    except: return False
    cur=None; ln=0; buf=[]; segs=[]
    for l in t.split('\n'):
        m=re.match(r'# (\d+) "([^"]*)"',l)
        if m:
            if buf: segs.append((cur,ln,'\n'.join(buf))); buf=[]
            cur=m.group(2); ln=int(m.group(1)); continue
        buf.append(l)
    if buf: segs.append((cur,ln,'\n'.join(buf)))
    pats=[r'typedef[^;{]*?(\w+)\s*(?:\[[^\]]*\])*;',r'}\s*(\w+)\s*;',r'\b(?:struct|union|enum)\s+(\w+)',r'\(\s*\*\s*(\w+)\s*\)\s*\(']
    for f,l0,tx in segs:
        for p in pats:
            for m in re.finditer(p,tx):
                POSMAP.setdefault(m.group(1),[]).append((f,l0+tx.count('\n',0,m.start())))
    return True
def vis_at(edit,line):
    v=set(['s8','s16','s32','u8','u16','u32','u_char','u_short','u_long','NULL'])
    for n,ls in POSMAP.items():
        for f,l in ls:
            if f!=edit or l<line: v.add(n); break
    return v
def pick(entries,kinds,pref_file=None):
    c=[e for e in entries if e['kind'] in kinds]
    if pref_file:
        c2=[e for e in c if e['file']==pref_file]
        if c2: c=c2
    # prefer header extern for vars
    def cp(e):
        a_,b_=e['file'].split('/'),CTXF[0].split('/'); n=0
        for x,y in zip(a_,b_):
            if x!=y: break
            n+=1
        return -n
    c.sort(key=lambda e:('__' in e['src'].split('\n')[0] and e['kind']=='typedef', cp(e) if a.pos else 0, not e['src'].lstrip().startswith('extern'), e['file'].endswith('.c')))
    return c[0] if c else None
def pick_var(es,n,src,pf):
    c=[e for e in es if e['kind']=='var']
    if not c: return None
    ss=strip_str(src)
    dot=bool(re.search(r'\b%s\s*\.'%re.escape(n),ss)); arrow=bool(re.search(r'\b%s\s*->'%re.escape(n),ss)); idx_=bool(re.search(r'\b%s\s*\['%re.escape(n),ss))
    def score(e):
        d=re.sub(r'\s*=.*','',e['src'],flags=re.S); sc=0
        arr='[' in d; ptr='*' in d
        if dot: sc+= 2 if not arr and not ptr else -2
        if arrow: sc+= 2 if (ptr or arr) else -2
        if idx_: sc+= 2 if (arr or ptr) else -2
        return sc
    c.sort(key=lambda e:(-score(e), e['file']!=pf, not e['src'].lstrip().startswith('extern'), e['file'].endswith('.c')))
    return c[0]
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
JT=[None,0]   # [jt-aware? , rodata start of current TU]
def jt_setup(tu):
    JT[0]=False; JT[1]=0
    cmds=run('ninja -t commands build/%s.s'%tu).stdout
    if '-jt/cc1' not in cmds: return
    try: mp=open(ROOT+'/build/out/%s.map'%MOD,errors='ignore').read().split('\n')
    except: return
    for i,l in enumerate(mp):
        m=re.match(r'^ \.rodata\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)$',l)
        if not m and re.match(r'^ \.rodata\s*$',l) and i+1<len(mp): m=re.match(r'^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)$',mp[i+1])
        if m and m.group(3)=='build/%s.o'%tu:
            JT[0]=True; JT[1]=int(m.group(1),16); return
def jt_markers(blizz_name):
    """one .xeno_jt marker per jump table of the retail function (mode 4 = tight, 8 = natural .align 3)"""
    if not JT[0]: return []
    fs=glob.glob("asm/%s/*matchings/**/%s.s"%(MOD,blizz_name),recursive=True)
    if not fs: return []
    t=open(fs[0]).read(); out=[]
    for m in re.finditer(r'dlabel jtbl_([0-9A-Fa-f]{8})\n(.*?)\.size jtbl_',t,re.S):
        a=int(m.group(1),16); n=len(re.findall(r'\.word',m.group(2)))
        out.append((a,'4' if (a-JT[1])%8==4 else '8',4*n))
    return ['__asm__(".xeno_jt %s %d jtbl_%08X");'%(md,tot,a) for a,md,tot in sorted(out)]
def analyze(row,fe,mode=''):
    """Return (block_text) or raise Reject."""
    name=row['xem_name']; src=fe['src']
    if re.search(r'\basm\b|__asm__',strip_str(src)): raise Reject('switch/float/asm')
    if re.search(r'\bswitch\b|\bfloat\b|\bdouble\b|\b\d+\.\d',strip_str(src)) or '"' in src:
        row['ro']=True
        if not has_ro(row['blizz_name']): raise Reject('rodata not migrated into function .s')
    pf=fe['file']; CTXF[0]=pf; decls=[]; typeents=[]; seen_t=set(); symdone={}; inprog=set(); fwd=set()
    base_toks=ident_tokens(src)
    for _m in list(base_toks):
        if _m in XMAC2: base_toks+= [t3 for t3 in ident_tokens(XMAC2[_m]) if t3!=_m]
    renames={}
    ISO='iso' in mode; ASMDEF='asmdef' in mode; tmap={}
    def isotype(n): return ISO and n not in PSYQ and any(e['kind'] in ('typedef','tag') for e in (idx.get(n,[])+idx.get('tag:'+n,[])))
    def add_type(n):
        if n in seen_t and n in inprog: fwd.add(n)
        if n in seen_t or (n in VIS and not isotype(n)): return
        if isotype(n): tmap[n]='xm_'+n
        seen_t.add(n)
        es=idx.get(n) or idx.get('tag:'+n)
        if not es: return
        e=pick(es,('typedef','define','tag'))
        if not e: return
        inprog.add(n)
        for t2 in ident_tokens(e['src']):
            if t2!=n: resolve(t2,e['src'],in_type=(e['kind']!='define'))
        inprog.discard(n)
        key=hashlib.md5(e['src'].encode()).hexdigest()
        if key not in [k for k,_ in typeents]: typeents.append((key,(n,e['src'])))
        if '{' not in e['src'] and re.match(r'\s*typedef\s+struct\s+\w+\s+\w+\s*;',e['src']):
            dfs=[x for x in idx.get('tag:'+n,[]) if '{' in x['src']]
            if dfs:
                inprog.add(n)
                for t2 in ident_tokens(dfs[0]['src']):
                    if t2!=n: resolve(t2,dfs[0]['src'],in_type=True)
                inprog.discard(n)
                key2=hashlib.md5(dfs[0]['src'].encode()).hexdigest()
                if key2 not in [k for k,_ in typeents]: typeents.append((key2,(n+'__def',dfs[0]['src'])))
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
        e=pick_var(es,n,src,pf)
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
    for i_,(key,(n,s)) in enumerate(typeents):
        for f_ in fwd:
            pos_=[j for j,(k2,(n2,s2)) in enumerate(typeents) if n2==f_]
            if pos_ and pos_[0]>i_ and re.search(r'typedef\s+struct\s+%s\b'%re.escape(f_),typeents[pos_[0]][1][1]):
                s=re.sub(r'(?<![\w.>])(?<!struct )%s\b(?=\s*\*)'%re.escape(f_),'struct '+f_,s)
        if s.lstrip().startswith('#'):
            for n0,r0 in renames.items(): s=re.sub(r'(?<![\w.>])%s\b'%re.escape(n0),r0,s)
            s=tren(s)
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
    um=[t for t in dict.fromkeys(ident_tokens(body)) if t in XMAC and not EA_MODE]
    k_=0
    while k_<len(um):
        for t2 in ident_tokens(XMAC[um[k_]].split('\n',1)[0].split(')',1)[-1]+'\n'+'\n'.join(XMAC[um[k_]].split('\n')[1:])):
            if t2 in XMAC and t2 not in um and t2!=um[k_]: um.append(t2)
        k_+=1
    def _rn(txt):
        for n0,r0 in renames.items(): txt=re.sub(r'(?<![\w.>])%s\b'%re.escape(n0),r0,txt)
        return txt
    for t in um: out.append('#pragma push_macro("%s")\n#undef %s\n%s'%(t,t,_rn(XMAC[t])))
    _ps=EA_MODE and re.search(r'PSX_SCRATCH\s*\(',body+''.join(o for o in out))
    if _ps: out.append('#pragma push_macro("PSX_SCRATCH")\n#undef PSX_SCRATCH\n#define PSX_SCRATCH(addr) (addr)')
    out.extend(jt_markers(row['blizz_name']) if not a.nojt else [])
    out.append(body)
    if _ps: out.append('#pragma pop_macro("PSX_SCRATCH")')
    for t in um: out.append('#pragma pop_macro("%s")'%t)
    out.append('#endif')
    return '\n'.join(out)+'\n'
XMAC={}
XMAC2={}
def _load_xmac():
    p=WORK+'/ext/xem/src/decomp/include/psyq/inline_c.h'
    try: t=open(p,errors='ignore').read()
    except: return
    for m in re.finditer(r'^#define (\w+)((?:[^\n]*\\\n)*[^\n]*)',t,re.M):
        XMAC[m.group(1)]=m.group(0)
def _load_xmac2():
    import glob as _g
    pat=re.compile(r"^#define (SCRIPT_VECTOR|SCRATCH_SVECTOR|GROUND_SCROLL|SPRITE_HIDDEN|VIEW_VECTORS|\w*_SCRATCH_?\w*)[ \t]+(\(\((?:\w+) \*\)(?:0x[0-9A-Fa-f]+|&\w+)\)|\d+)[ \t]*(?:/\*.*\*/)?[ \t]*$",re.M)
    for f in _g.glob(WORK+'/ext/xem/src/decomp/src/*/*.[hc]'):
        try: t=open(f,errors='ignore').read()
        except: continue
        for m in pat.finditer(t):
            XMAC.setdefault(m.group(1),'#define %s %s'%(m.group(1),m.group(2))); XMAC2[m.group(1)]=XMAC[m.group(1)]
_load_xmac2()
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
        elif t=='R_MIPS_LO16' and sec is None and nm0.startswith('D_1F80'): out.append('%08x'%((w&0xffff0000)|((ad+sx16(w&0xffff))&0xffff)))
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
HAND={}
if a.hand:
    cur=None
    for l in open(a.hand).read().split('\n'):
        m=re.match(r'^//// (\w+)\s*$',l)
        if m: cur=m.group(1); HAND[cur]=[]; continue
        if cur: HAND[cur].append(l)
    HAND={k:'#ifndef XENO_PC_PORT\n/* Hand-matched (references: ex-machina/EtherAura/Noah, m2c). */\n'+'\n'.join(v).strip('\n')+'\n#endif\n' for k,v in HAND.items()}
def main():
    if a.hand:
        rows=[{'blizz_name':k,'addr':'','insns':'0','xem_name':k,'xem_loc':'','xem_cc':'2.7.2'} for k in HAND]
    else:
      rows=[d for d in csv.DictReader(open(WORK+'/HARVEST.csv')) if d['module']==MOD and d['blizz_status']=='INCLUDE_ASM' and a.min_insns<=int(d['insns'])<=a.max_insns and ((d['ea_conf']=='aligned-by-order' and d['ea_func']) if EA_MODE else d['xem_status']=='C')]
    if EA_MODE and not a.hand:
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
        jt_setup(tu)
        edits=sorted(set(r['edit'] for r in rs)); origs={f:open(f).read() for f in edits}
        if a.prelude:
            for f in edits:
                if 'psyq/libgpu.h' not in origs[f] and '#include "common.h"\n' in origs[f]:
                    origs[f]=origs[f].replace('#include "common.h"\n','#include "common.h"\n#ifndef XENO_PC_PORT\n#include "psyq/libgpu.h"\n#endif\n',1)
                    open(f,'w').write(origs[f])
            run('nice -n 10 ninja -j4 build/%s.o'%tu)
        VIS.clear(); VIS.update(visible_for(tu)); VIS0=set(VIS)
        POS=build_posmap(tu) if a.pos else False
        def setvis(r):
            if not POS: return
            m=re.search(r'^[ \t]*INCLUDE_ASM\(\s*[^,]+,\s*%s\s*\)\s*;'%re.escape(r['blizz_name']),origs[r['edit']],re.M)
            ln=origs[r['edit']].count('\n',0,m.start())+1 if m else 10**9
            VIS.clear(); VIS.update(vis_at(r['edit'],ln)|MACROS[0])
        cands=[];
        for r in rs:
            if a.limit and n>=a.limit: break
            if a.hand:
                blk=HAND[r['blizz_name']]; r['mk']=(lambda blk=blk: (lambda md: blk))(); cands.append((r,blk)); n+=1; continue
            es=idx.get(r['xem_name'],[])
            fe=None
            for e in es:
                if e['kind']=='funcdef' and e['file']==r['xem_loc'].split(':')[0]: fe=e
            if not fe:
                res.append((r['blizz_name'],r['addr'],'reject','no funcdef')); continue
            try:
                setvis(r); blk=psyq_pre(analyze(r,fe))
            except Reject as ex:
                res.append((r['blizz_name'],r['addr'],'reject',str(ex))); continue
            r['mk']=(lambda r=r,fe=fe: (lambda md: (setvis(r),psyq_pre(analyze(r,fe,md)))[1]))()
            if a.dump and r['blizz_name']==a.dump:
                print(blk); print('=====ISO'); print(analyze(r,fe,'iso+asmdef'))
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
                        pp=attempt([(r0,nb)])
                        if pp.returncode==0: return [(r0,nb)]
                        if a.diag: print('CERR-'+md,r0['blizz_name'],pp.stdout[:1200])
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
                keep=[]
                for x in cs:
                    if attempt(keep+[x]).returncode==0: keep.append(x)
                    else: res.append((x[0]['blizz_name'],x[0]['addr'],'compile-error','conflicts with other adopted blocks in TU'))
                cs=keep
                if cs and attempt(cs).returncode!=0: res.append(('*TU*',tu,'abort','combined compile fails')); cs=[]
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
            if a.diag: print('ROUND',rnd,'cs',len(cs),'bad',[b[0]['blizz_name'] for b in bad][:12])
            if not bad: break
            order={k:i for i,k in enumerate(base)}
            bad.sort(key=lambda b:order.get(b[0]['blizz_name'],1<<30))
            sz=[i for i,b in enumerate(bad) if len(b)==2 and len(base.get(b[0]['blizz_name'],[]))!=len(new.get(b[0]['blizz_name'],[]))]
            if sz: bad=bad[:sz[0]+1]   # later diffs may only be shifted by this one; re-check next round
            elif JT[0] and len(bad)>1: bad=bad[:1]   # jt-aware TU: a failed table candidate shifts every later .rodata offset
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
