a217284(n) = n!^3*sum(k=0, n, 1/k!^3);
a(n) = if(n==0, 0, n^3*a217284(n-1));
for(n=0, 14, print1(a(n), ", "));
