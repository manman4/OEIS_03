\\ a(1) = 1; a(n) = Sum_{i,j,k,l,m >= 0 and i+j+k+l+m=n-2} (n-1)!/((i+1)!*j!*k!*l!*m!) * a(i+1)*a(j+1)*a(k+1)*a(l+1)*a(m+1).
a(n) = if(n==1, 1, sum(i=0, n-2, sum(j=0, n-2-i, sum(k=0, n-2-i-j, sum(l=0, n-2-i-j-k, my(m=n-2-i-j-k-l); (n-1)!/((i+1)!*j!*k!*l!*m!) * a(i+1)*a(j+1)*a(k+1)*a(l+1)*a(m+1))))));
for(n=1, 9, print1(a(n),", "));

