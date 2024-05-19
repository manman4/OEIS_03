a(n) = sumdiv(n, d, eulerphi(d)*sigma(d, 2));
for(n=1, 42, print1(a(n),", "))                 

b(n) = sum(k=1, n, sigma( n/gcd(n, k), 2));
for(n=1, 45,print1(a(n)-b(n),", "))

  