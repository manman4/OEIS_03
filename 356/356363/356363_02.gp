M=22;

a(n) = n!*polcoef(sum(k=0, n\3, n^k*(exp(x+x*O(x^n))-1)^(3*k)/(3*k)!), n);
for(n=0, M, print1(a(n), ", "));