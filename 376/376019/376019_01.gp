\\ a(n) = Sum_{d|n} d^n * binomial(n/d-1,d-1).
a(n) = sumdiv(n, d, d^n * binomial(n/d-1, d-1));
for(n=1, 29, print1(a(n),", ")) 

forprime(p=2, 100, print1(a(p)-1,", "))
