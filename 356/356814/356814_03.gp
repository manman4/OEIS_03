M=17;

a(n) = n!*polcoef(exp(x*(1-exp(n*x))), n);
for(n=0, M, print1(a(n), ", "));