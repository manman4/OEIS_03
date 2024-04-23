a(n) = 2*n!*sum(k=0, n\3, (k+2)^(n-2*k-1) / (k! * (n-3*k)!) );
for(n=0, 19, print1(a(n), ", "))

