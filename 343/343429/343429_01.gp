a(n) = my(A=1+O(x)); for(i=1, n, A=1+(n-i+1)^2*x/A); polcoef(A, n);
M=1000;
for(n=0, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b343429_1.txt", n, " ", i))
