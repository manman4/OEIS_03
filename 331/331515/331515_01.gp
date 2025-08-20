\\ a(n) = binomial(n+2,2) * A007564(n+1).
a007564(n) = (2^n*(pollegendre(n+1, 2)-pollegendre(n-1, 2)) + 2*(n==0))/(6*n+3);
a(n) = binomial(n+2,2) * a007564(n+1);
for(n=0, 20, print1(a(n),", "))  
