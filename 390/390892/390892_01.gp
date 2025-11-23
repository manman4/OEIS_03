M=13;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=-4*i*v[i]+16*sum(j=0, i-1, binomial(i, j+1)*v[j+1]*v[i-j])); v;
a_vector(14)

\\ a(0) = 1; a(n) = -4*n*a(n-1) + 16*Sum_{k=0..n-1} binomial(n,k+1) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, -4*n*a(n-1) + 16*sum(k=0, n-1, binomial(n,k+1) * a(k) * a(n-1-k)) );
for(n=0, M, print1(a(n),", "));

\\ a(n) = 4^n * A201466(n+1).
for(n=0, M, print1(a(n)/4^n,", "));
