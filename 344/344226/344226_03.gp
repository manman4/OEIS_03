M=70;

a(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d^n))/n;
for(n=1, M, print1(a(n), ", "));