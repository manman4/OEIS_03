\\ a(0) = 1; a(n) = Sum_{k=0..n-1} binomial(3*k+1,n-k-1) * a(k).
a(n) = if(n==0, 1, sum(k=0, n-1, binomial(3*k+1,n-k-1) * a(k)));
for(n=0, 10, print1(a(n),", "));

a_vector(n, s=3, t=1) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(k=0, i-1, binomial(s*k+t, i-k-1)*v[k+1])); v;
a_vector(30)