M=13;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=-2*i*v[i]+4*sum(j=0, i-1, binomial(i, j+1)*v[j+1]*v[i-j])); v;
a_vector(18)

\\ a(0) = 1; a(n) = -2*n*a(n-1) + 4*Sum_{k=0..n-1} binomial(n,k+1) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, -2*n*a(n-1) + 4*sum(k=0, n-1, binomial(n,k+1) * a(k) * a(n-1-k)) );
for(n=0, M, print1(a(n),", "));

\\ a(n) = 2^n * A000311(n+1).
for(n=0, M, print1(a(n)/2^n,", "));