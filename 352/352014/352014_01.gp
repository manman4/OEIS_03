M=25;

a(n) = sumdiv(n, d, isprime(d)*(-1)^(n/d+1)*(n-1)!/(d-1)!);
for(n=1, M, print1(a(n), ", "));