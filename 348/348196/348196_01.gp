M=74;

a(n) = sum(k=2, n^2-1, isprime(k)&&k%4==1);
for(n=1, M, print1(a(n), ", "));