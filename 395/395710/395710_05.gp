M=20;

\\ a(n) = (n+1) * Sum_{k=0..n} binomial(5*k+4+l,k) * binomial(5*n-5*k-l,n-k) for every real number l.
a(n, l) = (n+1)*sum(k=0, n, binomial(5*k+4+l, k) * binomial(5*n-5*k-l, n-k));
for(n=0, 10, print1(a(n, -4),", "));
for(l=-100, 100, for(n=0, 15, if(a(n, l/6)!=a(n, -4), print("Error: ", n, ", ", l))));

