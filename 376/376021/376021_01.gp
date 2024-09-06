\\ a(n) = Sum_{d|n} d^(n/d - d) * binomial(n/d-1,d-1).
a(n) = sumdiv(n, d, d^(n/d - d) * binomial(n/d-1, d-1));
for(n=1, 49, print1(a(n),", ")) 

forprime(p=2, 100, print1(a(p)-1,", "))
