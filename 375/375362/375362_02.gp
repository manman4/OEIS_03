\\ a(n) = Sum_{k=0..n} binomial(3*k-1,n-k).
a(n) = sum(k=0, n, binomial(3*k-1, n-k));                                                                           
for(n=0, 29, print1(a(n),", ")) 

\\ a(n) = Sum_{k=0..n} binomial(3*k-2,n-k).
a375363(n) = sum(k=0, n, binomial(3*k-2, n-k));
b(n) = if(n==0, 1, a375363(n) + a375363(n-1));
for(n=0, 100, print1(a(n) - b(n),", "))   