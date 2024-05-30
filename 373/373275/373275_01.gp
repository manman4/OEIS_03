a(n) = sumdiv(n, d, (-1)^(d-1)*2^(n/d-1));
for(n=1, 34, print1(a(n),", "))

forprime(p=2, 100, print1(a(p)-(1+2^(p-1)), ", "))