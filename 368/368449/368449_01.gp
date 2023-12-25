M=19;

\\ a(n) = 1 + Sum_{k=1..n} 2^(k-1) * (k-1)! * binomial(n,k) * a(n-k).
a(n) = 1 + sum(k=1, n, 2^(k-1) * (k-1)! * binomial(n, k) * a(n-k));
for(n=0, M, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=1+sum(j=1, i, 2^(j-1)*(j-1)!*binomial(i, j)*v[i-j+1])); v;
a_vector(M)
 