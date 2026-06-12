a(n,k,l) = if(k==0, 0^(n-1), a(n,k-1,l) + sum(j=1, n-1, binomial(n-1,j) * a(j,k+l-1,l) * a(n-j,k-1,l)));
for(n=1, 8, print1(a(n,1,5),", "));

