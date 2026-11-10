"""Top-level declaration index for the ex-machina C tree (src/decomp)."""
import re,os,glob,json,sys
KW=set('auto break case char const continue default do double else enum extern float for goto if int long register return short signed sizeof static struct switch typedef union unsigned void volatile while asm __asm__ inline'.split())
def strip_comments(t):
    t=re.sub(r'/\*.*?\*/',lambda m:'\n'*m.group(0).count('\n'),t,flags=re.S)
    t=re.sub(r'//[^\n]*','',t)
    return t
def chunks(text):
    """Yield (kind, names, src) for top-level chunks."""
    text=strip_comments(text)
    lines=text.split('\n'); i=0; out=[]; buf=[]; depth=0
    def flush():
        nonlocal buf
        s='\n'.join(buf).strip()
        if s: out.append(s)
        buf=[]
    while i<len(lines):
        l=lines[i]
        if depth==0 and not buf and l.lstrip().startswith('#'):
            s=l
            while s.rstrip().endswith('\\') and i+1<len(lines):
                i+=1; s+='\n'+lines[i]
            out.append(s.strip()); i+=1; continue
        if not buf and not l.strip():
            i+=1; continue
        buf.append(l)
        # update depth ignoring strings/chars
        for ch in re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'','',l):
            if ch=='{': depth+=1
            elif ch=='}': depth-=1
        st=l.strip()
        if depth==0 and (st.endswith(';') or st.endswith('}')):
            # a '}' ending: function def or struct w/o trailing ';'
            joined='\n'.join(buf)
            if st.endswith('}') and re.match(r'.*\)\s*\{',joined,re.S) and not re.search(r'\b(typedef|struct|union|enum)\b[^()]*\{',joined.split('{')[0]+'{'):
                flush()
            elif st.endswith(';'):
                flush()
            elif st.endswith('}'):
                pass  # struct body awaiting name;
        i+=1
    flush()
    return out
def classify(s):
    if s.startswith('#'):
        m=re.match(r'#\s*define\s+(\w+)',s)
        return ('define',[m.group(1)],s) if m else ('pp',[],s)
    if re.match(r'typedef\b',s):
        if '{' in s:
            m=re.search(r'\}\s*(\w+)\s*;\s*$',s)
            names=[m.group(1)] if m else []
            t=re.match(r'typedef\s+(?:struct|union|enum)\s+(\w+)\s*\{',s)
            if t: names.append('tag:'+t.group(1))
            return ('typedef',names,s)
        m=re.search(r'\(\s*\*\s*(\w+)\s*\)',s)
        if m: return ('typedef',[m.group(1)],s)
        m=re.search(r'(\w+)\s*(\[[^\]]*\])?\s*;\s*$',s)
        names=[m.group(1)] if m else []
        t=re.match(r'typedef\s+(?:struct|union|enum)\s+(\w+)\s*\{',s)
        if t: names.append('tag:'+t.group(1))
        return ('typedef',names,s)
    m=re.match(r'(?:struct|union|enum)\s+(\w+)\s*\{',s)
    if m: return ('tag',['tag:'+m.group(1)],s)
    head=s.split('{')[0]
    if '=' in head.split('(')[0] or (not '(' in head):
        # variable
        h=re.sub(r'=.*','',head,flags=re.S)
        ids=re.findall(r'(\w+)\s*(?:\[[^\]]*\])*\s*[,;]',h+';')
        m=re.search(r'(\w+)\s*(\[[^\]]*\])*\s*$',h.strip().rstrip(';'))
        return ('var',[m.group(1)] if m else [],s)
    m=re.search(r'(\w+)\s*\(',head)
    # function: name = last ident before first '(' not part of fn ptr
    m=re.match(r'[^()]*?(\w+)\s*\(',head,re.S)
    if '{' in s: return ('funcdef',[m.group(1)] if m else [],s)
    return ('proto',[m.group(1)] if m else [],s)
def build(root,dirs):
    idx={}
    files=[]
    for d in dirs: files+=glob.glob(os.path.join(root,d,'**','*.[ch]'),recursive=True)
    for f in sorted(files):
        t=open(f,errors='ignore').read()
        for s in chunks(t):
            k,names,src=classify(s)
            for n in names:
                idx.setdefault(n,[]).append({'kind':k,'src':src,'file':os.path.relpath(f,root)})
    return idx
if __name__=='__main__':
    root=sys.argv[1]
    idx=build(root,sys.argv[3].split(',') if len(sys.argv)>3 else ['src/decomp/include','src/decomp/src'])
    json.dump(idx,open(sys.argv[2],'w'))
    import collections; print(len(idx),collections.Counter(v[0]['kind'] for v in idx.values()))
