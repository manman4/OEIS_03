a(n) = sumdiv(n, d, (-1)^(d-1)*3^(n/d-1));
for(n=1, 28, print1(a(n),", "))

forprime(p=2, 100, print1(a(p)-(1+3^(p-1)), ", "))