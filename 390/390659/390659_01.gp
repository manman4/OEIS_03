M=20;
N=29;

\\ a(n) = 1 + 2 * Sum_{k=0..floor((n-1)/2)} a(k) * a(n-1-2*k).
a(n) = 1 + 2 * sum(k=0, (n-1)\2, a(k) * a(n-1-2*k)); 
for(n=0, M, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=1+2*sum(j=0, (i-1)\2, v[j+1]*v[i-2*j])); v;
a_vector(N)