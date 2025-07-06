M=10;

\\ a(0) = 1; a(n) = (n-1)! * Sum_{i, j, k, l>=0 and i+j+k+l=n-1} (n-i) * (2^j+1) * (2^k+1) * (2^l+1) * a(i) * a(j) * a(k) * a(l)/(i! * j! * k! * l!).
a(n) = if(n==0, 1, (n-1)! * sum(i=0, n-1, sum(j=0, n-1-i, sum(k=0, n-1-i-j, (n-i) * (2^j+1) * (2^k+1) * (2^(n-1-i-j-k)+1) * a(i) * a(j) * a(k) * a(n-1-i-j-k)/(i! * j! * k! * (n-1-i-j-k)!)))));
for(n=0, M, print1(a(n),", "));

