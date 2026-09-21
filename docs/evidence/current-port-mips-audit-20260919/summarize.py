from pathlib import Path
import json,collections,csv,re,subprocess,hashlib
base=Path('scratchpad/astra-port-audit-20260919');out=Path('docs/evidence/current-port-mips-audit-20260919')
x=json.loads((base/'comparison.json').read_text());native=json.loads((base/'native-symbols.json').read_text());root=str(Path.cwd())+'/'
rows=[]
for r in x['rows']:
 n=native['symbols'].get(r['function'],{});location=n.get('linked_source_location','')
 matches=[e for e in native.get('linked_entries',[]) if e['function']==r['function'] and e['linked_source_location'].split(':')[0].removeprefix(root)==r['source']]
 if matches: location=matches[0]['linked_source_location']
 path=location.split(':')[0].removeprefix(root)
 if r['function'] in native['stubs']: owner='GENERATED_STUB'
 elif path==r['source']: owner='SAME_SOURCE_LINKED'
 elif path.startswith('pc_port/'):owner='PORT_REPLACEMENT'
 elif path:owner='OTHER_SOURCE_OR_OVERLAY'
 else:owner='NO_LINKED_SYMBOL_BY_NAME'
 raw=r.get('raw_bytes',{});status=raw.get('status','NO_RETAIL_LISTING')
 if r['assembly_fallback'] and status=='SIZE_DIFFERENT':status='ASSEMBLY_SPAN_UNRESOLVED'
 rows.append(dict(source=r['source'],function=r['function'],overlay=r['overlay'],matching_build_body='ASM' if r['assembly_fallback'] else 'C',byte_status=status,retail_address=r.get('retail_address',''),retail_bytes=r.get('retail_bytes',''),compiled_code_bytes=r['mips_bytes'],native_owner=owner,linked_source=location,compiled_sha256=raw.get('compiled_sha256',''),retail_sha256=raw.get('retail_sha256',''),unresolved_reason=raw.get('reason','')))
with (out/'functions.csv').open('w') as f:
 w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
stubs=[]
for name in native['stubs']:
 candidates=[r for r in rows if r['function']==name]
 stubs.append(dict(function=name,matching_C_candidates=';'.join(r['source'] for r in candidates if r['matching_build_body']=='C'),matching_ASM_candidates=';'.join(r['source'] for r in candidates if r['matching_build_body']=='ASM'),linked_source=native['symbols'].get(name,{}).get('linked_source_location','')))
with (out/'stubs.csv').open('w') as f:
 w=csv.DictWriter(f,fieldnames=list(stubs[0]));w.writeheader();w.writerows(stubs)
missing=x['retail_functions_without_compiled_C_TU_symbol']
with (out/'retail-labels-outside-c-tu-symbols.csv').open('w') as f:
 w=csv.DictWriter(f,fieldnames=list(missing[0]));w.writeheader();w.writerows(missing)
build=Path('pc_port/build_port.sh').read_text();port=re.search(r'^PORT_SOURCES=\((.*?)^\)',build,re.M|re.S)[1];test=re.search(r'^TEST_ONLY_PORT_SOURCES=\((.*?)^\)',build,re.M|re.S)[1]
port_sources=re.findall(r'^\s+(pc_port/src/\S+\.c)(?:\s*#.*)?\s*$',port,re.M);test_sources=re.findall(r'^\s+(pc_port/src/\S+\.c)(?:\s*#.*)?\s*$',test,re.M)
with (out/'port-only-sources.csv').open('w') as f:
 w=csv.writer(f);w.writerow(['source','classification','matching_mips_byte_verification'])
 for p in port_sources+['pc_port/src/port_main.c']:w.writerow([p,'RUNTIME_PORT_SOURCE','NOT_APPLICABLE_HOST_ADAPTER_REQUIRES_DIFFERENTIAL_OR_RUNTIME_PROOF'])
 for p in test_sources:w.writerow([p,'TEST_ONLY','NOT_LINKED'])
summary={'head':subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip(),'translation_units':x['translation_units'],'function_definition_occurrences':len(rows),'unique_overlay_function_names':len({(r['overlay'],r['function']) for r in rows}),'authority':x['authority'],'counts':dict(collections.Counter(r['byte_status'] for r in rows)),'C_counts':dict(collections.Counter(r['byte_status'] for r in rows if r['matching_build_body']=='C')),'ASM_counts':dict(collections.Counter(r['byte_status'] for r in rows if r['matching_build_body']=='ASM')),'native_owner_counts':dict(collections.Counter(r['native_owner'] for r in rows)),'same_source_linked_C_counts':dict(collections.Counter(r['byte_status'] for r in rows if r['native_owner']=='SAME_SOURCE_LINKED' and r['matching_build_body']=='C')),'function_stubs':len(stubs),'stubs_with_C_candidate':sum(bool(r['matching_C_candidates']) for r in stubs),'port_runtime_sources':len(port_sources)+1,'test_only_port_sources':len(test_sources),'native_sha256':hashlib.sha256(Path('pc_port/build_native/xeno-port').read_bytes()).hexdigest()}
(out/'summary.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps({k:v for k,v in summary.items() if k!='authority'},indent=2))
