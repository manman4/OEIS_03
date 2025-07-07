M=15;

\\ a(0) = 1; a(n) = (n-1)! * Sum_{i, j, k>=0 and i+j+3*k=n-1} (n-i) * a(i) * a(j) * a(3*k)/(i! * j! * (3*k)!).
a(n) = if(n==0, 1, (n-1)! * sum(k=0, (n-1)\3, sum(j=0, n-1-3*k, (n-(n-1-3*k-j)) * a(n-1-3*k-j) * a(j) * a(3*k)/((n-1-3*k-j)! * j! * (3*k)!))));
for(n=0, M, print1(a(n),", "));