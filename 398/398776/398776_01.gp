\\ Square array read by antidiagonals downwards: A(n,k) = least m >= 1 such that k divides prime(n+1) + prime(n+2) + ... + prime(n+m), for n >= 0, k >= 1.
a(n, k) = my(m=0, s=0); until(s%k==0, m++; s+=prime(n+m)); m;
for(n=0, 12, for(k=0, n, print1(a(k,n-k+1),", ")));

