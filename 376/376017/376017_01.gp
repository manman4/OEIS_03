\\ a(n) = Sum_{d|n} d^(n/d - d) * binomial(n/d,d)
a(n) = sumdiv(n, d, d^(n/d - d) * binomial(n/d, d));
for(n=1, 45, print1(a(n),", ")) 

forprime(p=2, 100, print1(a(p)-p,", "))
