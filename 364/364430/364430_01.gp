a(n) = (-1)^n*sum(k=0, n, (-2)^k*binomial(n, k)*binomial(n+3*k+1, n)/(n+3*k+1));
for(n=0, 20, print1(a(n),", ")) 