M=18;

a(n) = if(n==0, 1, n!*polcoef(n/(n-1+exp(n*x+x*O(x^n))), n));
for(n=0, M, print1(a(n),", "))
