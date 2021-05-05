M=10;

a(n) = x='x+O('x^(n+1)); polcoef(sum(k=0, n, k!^n*x^k/(1-x)^(k+1)), n) 
for(n=0, M, print1(a(n), ", "));