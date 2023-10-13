M=10;

a(n) = x='x+O('x^(n+1)); n!*polcoef(exp(x)*sum(k=0, n, k!^n*x^k), n) 
for(n=0, M, print1(a(n), ", "));