M=73;

a(n) = sum(k=2, n^2-1, isprime(k)&&k%4==3);
for(n=1, M, print1(a(n), ", "));