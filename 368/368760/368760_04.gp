a337001(n) = n! * sum(k=0, n, k^3 / k!);
a(n) = n! + a337001(n);
for(n=0, 21, print1(a(n), ", "));  
