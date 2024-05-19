a(n) = sumdiv(n, d, eulerphi(d)*sigma(d^3));
for(n=1, 36, print1(a(n),", "))                 

for(n=1, 100, if(a(n)==n^4, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^3, 1));
for(n=1, 45,print1(a(n)-b(n),", "))

  