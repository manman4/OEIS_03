M=16;

\\ a(n) = 1 + Sum_{k=0..n-1} (1 + 2*k - 3*k^2 + k^3) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, 1 + sum(k=0, n-1, (1 + 2*k - 3*k^2 + k^3) * a(k) * a(n-1-k) ));   
for(n=0, M, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=1+sum(j=0, i-1, (1+sum(k=1, 3, stirling(3, k, 1)*j^k))*v[j+1]*v[i-j])); v;
a_vector(M)  

