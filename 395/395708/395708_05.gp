M=20;

\\ a(n) = (n+1) * Sum_{k=0..n} binomial(3*k+2+l,k) * binomial(3*n-3*k-l,n-k) for every real number l.
a(n, l) = (n+1)*sum(k=0, n, binomial(3*k+2+l, k) * binomial(3*n-3*k-l, n-k));
for(n=0, 10, print1(a(n, -2),", "));
for(l=-100, 100, for(n=0, 15, if(a(n, l/6)!=a(n, -2), print("Error: ", n, ", ", l))));

