a(n) = sumdiv(n, d, eulerphi(d)*sigma(d^2));
for(n=1, 42, print1(a(n),", "))                 

for(n=1, 100, if(a(n)==n^3, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^2, 1));
for(n=1, 45,print1(a(n)-b(n),", "))

  