a(n) = sumdiv(n, d, isprime(d)*d^(n/d-1));

for(n=1, 50, print1(a(n),", "))    