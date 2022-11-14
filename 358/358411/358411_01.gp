M=22;

a(n) = sumdiv(n, d, (d+n/d-1)!/(d-1)!);
for(n=1, M, print1(a(n), ", "));