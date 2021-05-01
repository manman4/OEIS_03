M=30;

a(n) = x='x+O('x^(n+1)); (n-1)!*polcoef(exp(x/(1-x))/(1-x)^(n+1), n-1) 
for(n=1, M, print1(a(n), ", "));