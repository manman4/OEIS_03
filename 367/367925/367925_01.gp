M=15;
N=17;

\\ a(0) = 1; a(n) = n * a(n-1) + 3 * Sum_{k=1..n} binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, n*a(n-1) + 3 * sum(k=1, n, binomial(n,k) * a(n-k)));
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*v[i]+3*sum(j=1, i, binomial(i, j)*v[i-j+1])); v;
a_vector(N)