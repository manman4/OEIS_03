a374506(n) = binomial(n+6, 3)/20*sum(k=0, n\2, binomial(n+3, n-2*k)*binomial(2*k+3, k));
a(n) = if(n==0, a374506(0), a374506(n)-a374506(n-1));
for(n=0, 24, print1(a(n),", "))




 