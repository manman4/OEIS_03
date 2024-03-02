M=25;

b(n) = my(x='x+O('x^(n+2))); polcoef( ( 1/(1-x-x^4) )^(n+1), n)/(n+1);
for(n=0, M, print1(b(n), ", "))

a(n) = sum(k=0, n, binomial(n,k)*b(k));
for(n=0, M, print1(a(n), ", "))