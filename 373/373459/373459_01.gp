a(n) = sumdiv(n, d, isprime(d)*d^(n-d));

for(n=1, 32, print1(a(n),", "))    