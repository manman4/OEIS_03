M=25;

a(n) = if(n==-2, 1, -sum(k=1, n+2, isprime(k+2)*a(n-k)));
for(n=-2, M, print1(a(n), ", "));
