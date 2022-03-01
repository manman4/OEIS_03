M=25;

a(n) = sumdiv(n, d, isprime(d)*(n-1)!/(d-1)!); 
for(n=1, M, print1(a(n), ", "));