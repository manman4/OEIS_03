\\ A(n,k) = Pell(k*n)/Pell(k) for k > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
a(n, k) = if(k==0, n, pell(k*n)/pell(k));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));