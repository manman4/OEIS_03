M=30;

a(n) = x='x+O('x^(n+1)); n!*polcoef(exp(-x/(1-x))/(1-x)^(n+2), n) 
for(n=0, M, print1(a(n), ", "));