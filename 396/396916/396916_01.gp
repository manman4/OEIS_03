\\ a(0) = 1; a(n) = n * Sum_{k=0..n-1} (k+2)^(n-k-1) * binomial(n-1,k) * a(k).
a(n) = if(n==0, 1, n * sum(k=0, n-1, (k+2)^(n-k-1) * binomial(n-1,k) * a(k)));
for(n=0, 10, print1(a(n),", "));

a_vector(n, s=1, t=2) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*sum(k=0, i-1, (s*k+t)^(i-k-1)*binomial(i-1, k)*v[k+1])); v;
a_vector(30)