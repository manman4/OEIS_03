a(n) = sumdiv(n, d, eulerphi(d)*sigma(d^n));
for(n=1, 18,print1(a(n),", "))                 

for(n=1, 100, if(a(n)==n^(n+1), print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^n, 1));
for(n=1, 45,print1(a(n)-b(n),", "))

  