a(n) = sumdiv(n, d, eulerphi(d)*sigma(d^4));
for(n=1, 33, print1(a(n),", "))                 

for(n=1, 100, if(a(n)==n^5, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^4, 1));
for(n=1, 45,print1(a(n)-b(n),", "))

  