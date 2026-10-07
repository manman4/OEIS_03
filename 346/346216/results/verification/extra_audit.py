"""Independent oracle against small/non-coprime ratios and forced fallbacks."""
from pathlib import Path
import hashlib, json, math, random, subprocess, sys, tempfile
binary=Path(sys.argv[1]).resolve()
# Build every pair sum independently; no membership compression or residue pruning.
limit=50000
counts=bytearray(limit+1)
terms=[1,2]
counts[3]=1
for candidate in range(3,limit+1):
    if counts[candidate]==1:
        for earlier in terms:
            total=earlier+candidate
            if total>limit: break
            counts[total]=min(2,counts[total]+1)
        terms.append(candidate)
    if len(terms)==2000: break
assert len(terms)==2000
hashes=[14695981039346656037]
for term in terms: hashes.append(((hashes[-1]^term)*1099511628211)%(1<<64))
ratios=[(p,q) for p in range(2,49) for q in range((p+2)//3,p//2+1)]
ratios.extend([(1000000000,400000000),(999999999,333333333),(999999998,499999999),
               (856371966,350477575),(44,18),(220000000,90000000)])
rng=random.Random(7102026)
records=[]
with tempfile.TemporaryDirectory(prefix='ulam02-ratios-') as temporary:
    work=Path(temporary)
    for index,(p,q) in enumerate(ratios):
        count=2000 if index%17==0 else 1000
        cap=rng.choice([0,1,4,23,500000])
        window=rng.choice([1,2,19,100000])
        threshold=rng.choice([0,1,100,1000000])
        dictionary=rng.choice([1,2,255])
        args=[str(binary),'--no-bfile','--no-summary','--terms',str(count),'--report','0',
              '--p',str(p),'--q',str(q),'--anchor-limit',str(cap),'--window',str(window),
              '--threshold',str(threshold),'--dictionary-limit',str(dictionary),
              '--verify-until','50000']
        if index%29==0:
            args+=['--disk',str(work/f'cache_{index}.bin'),'--cache-mib','1']
        else: args+=['--ram']
        result=subprocess.run(args,cwd=work,capture_output=True,text=True,timeout=30)
        assert result.returncode==0,(index,args,result.stderr)
        assert 'runtime error:' not in result.stderr and 'UndefinedBehaviorSanitizer' not in result.stderr
        stats=json.loads(result.stderr.splitlines()[-1])
        assert stats['value']==terms[count-1] and stats['digest64']==hashes[count]
        rows=''.join(f'{i} {terms[10**i-1]}\n' for i in range(len(str(count))))
        assert result.stdout==rows
        low=sum(3*((q*x)%p)<p for x in terms[:count])
        high=sum(3*((q*x)%p)>2*p for x in terms[:count])
        assert stats['residue_outliers_low']==low and stats['residue_outliers_high']==high
        records.append({'p':p,'q':q,'gcd':math.gcd(p,q),'terms':count,
                        'anchor_limit':cap,'window':window,'threshold':threshold,
                        'dictionary_limit':dictionary,'fallback_calls':stats['fallback_calls'],
                        'escaped_blocks':stats['escaped_blocks']})
Path('ratio_results.json').write_text(json.dumps(records,indent=2)+'\n')
print(f'PASS {len(records)} ratio/configuration cases; every supported ratio with p=2..48 included.')
print('Non-coprime ratios, endpoint ratios 2 and 3, forced fallback/raw escapes, exact outlier counts agree.')
