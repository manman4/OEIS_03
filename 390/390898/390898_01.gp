M=12;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=-2*v[i]+2*sum(j=0, i-1, binomial(i+1, j+1)*v[j+1]*v[i-j])); v;
a_vector(17)

\\ a(0) = 1; a(n) = -2*a(n-1) + 2*Sum_{k=0..n-1} binomial(n+1,k+1) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, -2*a(n-1) + 2*sum(k=0, n-1, binomial(n+1,k+1) * a(k) * a(n-1-k)) );
for(n=0, M, print1(a(n),", "));

\\ a(n) = 2^n * A032188(n+1).
for(n=0, M, print1(a(n)/2^n,", "));