M=14;

a(n) = n!*polcoef(1/(1-sum(k=1, n, (exp(k*x+x*O(x^n))-1)/k)), n);
for(n=0, M, print1(a(n), ", "));