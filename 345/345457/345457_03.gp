\\ a(n) = Sum_{k=0..n} binomial(5*n+3,5*k+3). 
a(n) = sum(k=0, n, binomial(5*n+3, 5*k+3));
for(n=0, 20, print1(a(n),", "));

