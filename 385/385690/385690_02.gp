M=10;

\\ a(0) = 1; a(n) = (n-1)! * Sum_{i, j, k>=0 and i+j+2*k=n-1} (n-i) * a(i) * a(j) * a(2*k)/(i! * j! * (2*k)!).
a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-1)\2, sum(j=0, n-1-2*k, (n-(n-1-2*k-j)) * a(n-1-2*k-j) * a(j) * a(2*k)/((n-1-2*k-j)! * j! * (2*k)!))));
for(n=0, M, print1(a(n),", "));