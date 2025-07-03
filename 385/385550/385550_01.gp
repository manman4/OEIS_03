M=17;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (k-n)^k * binomial(n,k) * a(k).
a(n) = if(n==0, 1, sum(k=0, n-1, (k-n)^k * binomial(n,k) * a(k)));
for(n=0, M, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, (j-i)^j * binomial(i, j) * v[j+1])); v;
a_vector(M)