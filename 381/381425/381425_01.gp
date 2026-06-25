M=11;

C(n, k) = if(k>=0, binomial(n, k));
a(n, k) = sum(j=0, k, C(k, j)*C(n+(k-1)*j-1, n-j));
for(n=0, M, for(k=0, n, print1(a(k, n-k)", ")));






