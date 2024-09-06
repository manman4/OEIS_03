\\ a(n) = Sum_{d|n} d^n * binomial(n/d,d).
a(n) = sumdiv(n, d, d^n * binomial(n/d, d));
for(n=1, 28, print1(a(n),", ")) 

forprime(p=2, 100, print1(a(p)-p,", "))
