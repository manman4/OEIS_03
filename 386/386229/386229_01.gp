\\ a(n) = (n+1) * (1 + 1/2 * Sum_{k=0..n-1} a(k) * a(n-1-k)).
a(n) = (n+1) * (1 + 1/2 * sum(k=0, n-1, a(k) * a(n-1-k)));
for(n=0, 13, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=(i+1)*(1+sum(j=0, i-1, v[j+1]*v[i-j])/2)); v;
a_vector(20)
