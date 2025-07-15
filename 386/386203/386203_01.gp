M=27;

\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/4)} binomial(n-1,4*k) * a(4*k) * a(n-1-4*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\4, binomial(n-1,4*k) * a(4*k) * a(n-1-4*k)));
for(n=0, 20, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\4, binomial(i-1, 4*j)*v[4*j+1]*v[i-4*j])); v;
a_vector(M)