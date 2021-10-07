M=105;

a(n) = sum(k=n^2, (n+1)^2, (isprime(k)&&k%4==3)-(isprime(k)&&k%4==1));
for(n=1, M, print1(a(n), ", "));