M=15;

a(n) = n!* polcoef(1/(1 - n * x)^x, n);
for(n=0, M, print1(a(n),", "))
