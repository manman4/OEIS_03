M=40;

a(n) = my(x='x+x*O('x^n)); polcoef(sum(k=1, n, eulerphi(k)*x^k/(1-x^k)^(n+1)), n);
for(n=1, M, print1(a(n),", "));  