a(n) = my(A=1+O(x)); for(i=1, n, A=1+2^(n-i)*x/A); polcoef(A, n);
M=100;
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b343439_1.txt", n, " ", i))