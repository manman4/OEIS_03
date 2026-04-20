\\ a(n) = 1 - (1/2) * Sum_{k=0..n-1} (-4)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = 1 - (1/2) * sum(k=0, n-1, (-4)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 19, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=1 - (1/2) * sum(j=0, i-1, (-4)^(i-j) * binomial(2*i, 2*j) * v[j+1])); v;
a_vector(12)