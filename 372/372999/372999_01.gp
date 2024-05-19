a(n) = sumdiv(n, d, eulerphi(d)*numdiv(d^4));
for(n=1, 54,print1(a(n),", "))                 

for(n=1, 100, if(a(n)==5*n-4, print1(n,", ")))  

b(n) = sum(k=1, n, sigma( (n/gcd(n, k))^4, 0));
for(n=1, 45,print1(a(n)-b(n),", "))

  