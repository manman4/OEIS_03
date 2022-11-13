M=22;

a(n) = n*sumdiv(n, d, (d+n/d-2)!/d!);
for(n=1, M, print1(a(n), ", "));