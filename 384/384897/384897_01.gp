M=24;

a(n, k=-1) = if(n*k==0, 0^n, (-1)^n*k*sum(j=1, n, binomial(-2*n+3*j+k-1, j-1)*a(n-j, 3*j)/j));
for(n=0, M, print1(a(n),", "));

