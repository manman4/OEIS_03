M=16;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (1 - 6*k + 11*k^2 - 6*k^3 + k^4) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, sum(k=0, n-1, (1 - 6*k + 11*k^2 - 6*k^3 + k^4) * a(k) * a(n-1-k) ));   
for(n=0, M, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, (1+sum(k=1, 4, stirling(4, k, 1)*j^k))*v[j+1]*v[i-j])); v;
a_vector(M)  

