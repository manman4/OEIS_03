"""Reproduce early diagnostic writes to an aliased b-file without truncating it."""
from pathlib import Path
import tempfile,subprocess,json,sys
binary=Path(sys.argv[1]).resolve()
records=[]
with tempfile.TemporaryDirectory(prefix='ulam02-early-error-') as tmp:
    d=Path(tmp)
    for flags in [['--terms','0'],['--window','0'],['--unknown','1']]:
        bfile=d/'b.txt';original=b'0 1\n1 18\n';bfile.write_bytes(original)
        with bfile.open('r+') as stream:
            r=subprocess.run([str(binary),'--ram','--bfile',str(bfile),*flags],cwd=d,
                             stdout=subprocess.PIPE,stderr=stream,text=True,timeout=10)
        changed=bfile.read_bytes()!=original
        assert r.returncode==1 and changed
        records.append({'flags':flags,'returncode':r.returncode,
                        'before':original.decode(),'after':bfile.read_text(),'changed':changed})
    bfile=d/'b.txt';bfile.write_bytes(original)
    with bfile.open('r+') as stream:
        r=subprocess.run([str(binary),'--ram','--bfile',str(bfile),'--terms','10'],cwd=d,
                         stdout=subprocess.PIPE,stderr=stream,text=True,timeout=10)
    assert r.returncode==1 and bfile.read_bytes()==original
    records.append({'flags':['--terms','10'],'returncode':r.returncode,'changed':False})
Path('early_error_results.json').write_text(json.dumps(records,indent=2)+'\n')
print('CONFIRMED early-error finding: invalid arguments overwrite stderr-aliased b-file; valid arguments preserve it.')
