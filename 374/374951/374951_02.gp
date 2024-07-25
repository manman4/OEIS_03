a(n) = sum(i=1, n, sum(j=1, n-i, sum(k=1, n-i-j, (i+j+k==n)*sigma(i)*sigma(j)*sigma(k))));
for(n=1, 38, print1(a(n),", "))

