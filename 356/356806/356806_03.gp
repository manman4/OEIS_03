M=17;

a(n) = n!*polcoef(exp(x*(exp(n*x)-1)), n);
for(n=0, M, print1(a(n), ", "));