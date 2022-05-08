M=30;

a(n) = if(n==0, 1, stirling(2*n, n, 2)+binomial(2*n, n)/2);
for(n=0, M, print1(a(n), ", "));