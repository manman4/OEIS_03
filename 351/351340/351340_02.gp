M=20;

a(n) = polcoef(sum(k=0, n, (n*x)^k/(1-k*x+x*O(x^n))), n);
for(n=0, M, print1(a(n), ", "));