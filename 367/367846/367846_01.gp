M=15;
N=16;

\\ a(0) = 1; a(n) = n * a(n-1) + Sum_{k=1..n} 3^k * (k-1)! * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, n*a(n-1) + sum(k=1, n, 3^k * (k-1)! * binomial(n,k) * a(n-k)));
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*v[i]+sum(j=1, i, 3^j*(j-1)!*binomial(i, j)*v[i-j+1])); v;
a_vector(N)