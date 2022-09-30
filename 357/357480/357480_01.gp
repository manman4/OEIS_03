M=23;

a(n) = n!/24*sum(k=0, n-4, 1/k!);
for(n=0, M, print1(a(n), ", "));